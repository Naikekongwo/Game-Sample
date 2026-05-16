"""
OCMP Map Editor — 基于 Tkinter 的地图编辑器
用于编辑 OpenCore .ocmp 地图文件

.ocmp 二进制格式:
  MapHeader (36 bytes, MSVC aligned):
    magic[4]   = "OCMP"
    version    = uint8  (offset 4)
    [padding]            (offset 5)
    width      = uint16 (offset 6, LE)
    height     = uint16 (offset 8, LE)
    layerCount = uint8  (offset 10)
    blockSize  = uint8  (offset 11)
    reserved[24]         (offset 12-35)
  Data: width * height blocks, each blockSize bytes
    BlockInfo { Terrain:u8, STRuct:u8, Entity:u8, Access:u8 }
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox, simpledialog
import struct
import os
import sys

# ---------------------------------------------------------------------------
# 颜色映射 — 根据值可视化 BlockInfo 四个字段
# ---------------------------------------------------------------------------
TERRAIN_COLORS = {
    0: "#1a1a2e", 1: "#16213e", 2: "#0f3460", 3: "#533483",
    4: "#2d6a4f", 5: "#40916c", 6: "#52b788", 7: "#95d5b2",
    8: "#e9c46a", 9: "#f4a261", 10: "#e76f51",
}

STRUCT_COLORS = {
    0: "#222222", 1: "#3d3d3d", 2: "#555555", 3: "#8b4513",
    4: "#a0522d", 5: "#6b8e23", 6: "#4682b4", 7: "#8b0000",
}

ENTITY_COLORS = {
    0: "#222222", 1: "#ff6b6b", 2: "#555555", 3: "#ffd93d",
    4: "#6bcb77", 5: "#4d96ff",
}

ACCESS_COLORS = {
    0: "#ff4444", 1: "#44ff44", 2: "#555555",
}


def color_for(field, val):
    palette = {0: TERRAIN_COLORS, 1: STRUCT_COLORS, 2: ENTITY_COLORS, 3: ACCESS_COLORS}[field]
    return palette.get(val, "#888888")


# ---------------------------------------------------------------------------
# .ocmp 文件读写
# ---------------------------------------------------------------------------

def load_ocmp(filepath):
    """返回 (width, height, blocks, blockSize).
       blocks 是 list[list[list[int]]]，shape: height × width × 4
    """
    with open(filepath, "rb") as f:
        data = f.read()

    # 尝试 aligned header (sizeof=36, MSVC 默认)
    magic, ver, pad, w, h, lc, bs = struct.unpack_from("<4sBBHHBB", data, 0)
    if magic == b"OCMP" and 0 < w < 10000 and 0 < h < 10000 and bs in range(1, 17):
        header_sz = 36
    else:
        # 尝试 packed header (sizeof=35)
        magic, ver, w, h, lc, bs = struct.unpack_from("<4sBHHBB", data, 0)
        if magic == b"OCMP" and 0 < w < 10000 and 0 < h < 10000 and bs in range(1, 17):
            header_sz = 35
        else:
            raise ValueError(f"无法解析文件头: magic={magic}")

    offset = header_sz
    blocks = []
    for y in range(h):
        row = []
        for x in range(w):
            raw = data[offset:offset + bs]
            if len(raw) < 4:
                raw = raw + b'\x02' * (4 - len(raw))
            row.append(list(raw[:4]))
            offset += bs
        blocks.append(row)
    return w, h, blocks, bs, ver, lc


def save_ocmp(filepath, width, height, blocks, blockSize=4, version=1, layerCount=1):
    """写入 .ocmp 文件（aligned header, 36 bytes）"""
    header = struct.pack("<4sBBHHBB24s",
                         b"OCMP", version, 0,  # 0 = padding byte
                         width, height,
                         layerCount, blockSize,
                         b'\x00' * 24)
    with open(filepath, "wb") as f:
        f.write(header)
        for row in blocks:
            for cell in row:
                raw = bytes(cell[:4]) + b'\x02' * max(0, blockSize - 4)
                f.write(raw[:blockSize])


# ---------------------------------------------------------------------------
# 主编辑器
# ---------------------------------------------------------------------------

CELL_SIZE = 18  # 基础格大小
MIN_CELL = 6
MAX_CELL = 48


class MapEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("OCMP Map Editor")
        self.root.geometry("1280x820")

        # 数据
        self.width = 0
        self.height = 0
        self.blocks = []       # [row][col] = [Terrain, STRuct, Entity, Access]
        self.blockSize = 4
        self.filepath = None
        self.modified = False

        # 画笔
        self.brush_values = [2, 2, 2, 2]
        self.view_mode = 0  # 0=综合(Terrain主色+Access边框), 1=Terrain, 2=STRuct, 3=Entity, 4=Access

        # 视口
        self.offset_x = 0
        self.offset_y = 0
        self.cell_size = CELL_SIZE
        self.selected_cell = None  # (col, row)

        self._build_ui()
        self._update_title()

    # ------------------------------------------------------------------
    # UI 构建
    # ------------------------------------------------------------------
    def _build_ui(self):
        # 菜单栏
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        file_menu = tk.Menu(menubar, tearoff=0)
        file_menu.add_command(label="新建 (Ctrl+N)", command=self._new_map, accelerator="Ctrl+N")
        file_menu.add_command(label="打开 (Ctrl+O)", command=self._open_map, accelerator="Ctrl+O")
        file_menu.add_command(label="保存 (Ctrl+S)", command=self._save_map, accelerator="Ctrl+S")
        file_menu.add_command(label="另存为...", command=self._save_as)
        file_menu.add_separator()
        file_menu.add_command(label="退出", command=self.root.quit)
        menubar.add_cascade(label="文件", menu=file_menu)

        view_menu = tk.Menu(menubar, tearoff=0)
        view_menu.add_command(label="放大 (+)", command=lambda: self._zoom(1))
        view_menu.add_command(label="缩小 (-)", command=lambda: self._zoom(-1))
        view_menu.add_command(label="重置视图", command=self._reset_view)
        view_menu.add_separator()
        view_menu.add_command(label="清空地图 (全填充默认值)", command=self._fill_default)
        menubar.add_cascade(label="视图", menu=view_menu)

        # 快捷键
        self.root.bind("<Control-n>", lambda e: self._new_map())
        self.root.bind("<Control-o>", lambda e: self._open_map())
        self.root.bind("<Control-s>", lambda e: self._save_map())
        self.root.bind("<plus>", lambda e: self._zoom(1))
        self.root.bind("<minus>", lambda e: self._zoom(-1))
        self.root.bind("<Control-plus>", lambda e: self._zoom(1))
        self.root.bind("<Control-minus>", lambda e: self._zoom(-1))

        # 主布局
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True)

        # 左侧属性面板
        left_panel = ttk.Frame(main_frame, width=260)
        left_panel.pack(side=tk.LEFT, fill=tk.Y, padx=(4, 0), pady=4)
        left_panel.pack_propagate(False)

        self._build_properties(left_panel)

        # 右侧画布区域
        right_panel = ttk.Frame(main_frame)
        right_panel.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=2, pady=4)

        # 工具栏
        self._build_toolbar(right_panel)

        # 画布
        canvas_frame = ttk.Frame(right_panel)
        canvas_frame.pack(fill=tk.BOTH, expand=True)

        self.canvas = tk.Canvas(canvas_frame, bg="#1a1a1a", highlightthickness=0)
        self.canvas.pack(fill=tk.BOTH, expand=True)

        # 滚动条
        self.h_scroll = ttk.Scrollbar(canvas_frame, orient=tk.HORIZONTAL, command=self.canvas.xview)
        self.h_scroll.pack(side=tk.BOTTOM, fill=tk.X)
        self.v_scroll = ttk.Scrollbar(canvas_frame, orient=tk.VERTICAL, command=self.canvas.yview)
        self.v_scroll.pack(side=tk.RIGHT, fill=tk.Y)

        self.canvas.configure(xscrollcommand=self.h_scroll.set, yscrollcommand=self.v_scroll.set)

        # 画布事件
        self.canvas.bind("<Button-1>", self._on_click)
        self.canvas.bind("<B1-Motion>", self._on_drag)
        self.canvas.bind("<ButtonRelease-1>", self._on_release)
        self.canvas.bind("<MouseWheel>", self._on_mousewheel)
        self.canvas.bind("<Button-2>", self._on_middle_click)
        self.canvas.bind("<B2-Motion>", self._on_middle_drag)

        # 状态栏
        status = ttk.Frame(self.root)
        status.pack(side=tk.BOTTOM, fill=tk.X)
        self.status_label = ttk.Label(status, text="就绪", anchor=tk.W)
        self.status_label.pack(side=tk.LEFT, padx=4)
        self.cell_label = ttk.Label(status, text="", anchor=tk.E)
        self.cell_label.pack(side=tk.RIGHT, padx=4)

    def _build_properties(self, parent):
        """左侧属性编辑面板"""
        ttk.Label(parent, text="属性编辑器", font=("", 11, "bold")).pack(pady=(8, 12))

        # 地图信息
        info_frame = ttk.LabelFrame(parent, text="地图信息", padding=6)
        info_frame.pack(fill=tk.X, padx=6, pady=(0, 8))
        self.info_label = ttk.Label(info_frame, text="未加载地图")
        self.info_label.pack()

        # 视图模式
        mode_frame = ttk.LabelFrame(parent, text="显示模式", padding=6)
        mode_frame.pack(fill=tk.X, padx=6, pady=(0, 8))
        self.mode_var = tk.IntVar(value=0)
        modes = [("综合 (Terrain+Access)", 0), ("仅 Terrain", 1),
                 ("仅 STRuct", 2), ("仅 Entity", 3), ("仅 Access", 4)]
        for text, val in modes:
            ttk.Radiobutton(mode_frame, text=text, variable=self.mode_var,
                            value=val, command=self._on_mode_change).pack(anchor=tk.W)

        # 当前选中格
        sel_frame = ttk.LabelFrame(parent, text="选中格子", padding=6)
        sel_frame.pack(fill=tk.X, padx=6, pady=(0, 8))
        self.sel_pos_label = ttk.Label(sel_frame, text="(无)")
        self.sel_pos_label.pack()

        # 四个字段编辑
        fields_frame = ttk.LabelFrame(parent, text="字段值", padding=6)
        fields_frame.pack(fill=tk.X, padx=6, pady=(0, 8))

        self.field_vars = []
        self.field_spins = []
        labels = ["Terrain (地形)", "STRuct (结构)", "Entity (实体)", "Access (通行)"]
        for i, lbl in enumerate(labels):
            ttk.Label(fields_frame, text=lbl).pack(anchor=tk.W)
            var = tk.IntVar(value=2)
            spin = ttk.Spinbox(fields_frame, from_=0, to=255, textvariable=var,
                               width=8, command=lambda idx=i: self._on_field_change(idx))
            spin.pack(fill=tk.X, pady=(0, 4))
            var.trace_add("write", lambda *a, idx=i: self._on_field_change(idx))
            self.field_vars.append(var)
            self.field_spins.append(spin)

        # 应用到选中格
        ttk.Button(parent, text="应用当前值到选中格", command=self._apply_to_selected).pack(pady=2)

        # 画笔预设
        brush_frame = ttk.LabelFrame(parent, text="画笔预设", padding=6)
        brush_frame.pack(fill=tk.X, padx=6, pady=(8, 0))

        presets = [
            ("可通行地板", [3, 0, 2, 1]),
            ("墙壁/阻挡", [4, 0, 2, 0]),
            ("水/障碍", [6, 0, 2, 0]),
            ("空地(默认)", [2, 2, 2, 2]),
        ]
        for name, vals in presets:
            ttk.Button(brush_frame, text=name,
                       command=lambda v=vals: self._set_brush(v)).pack(fill=tk.X, pady=1)

        # 图例
        legend_frame = ttk.LabelFrame(parent, text="图例 (Access)", padding=6)
        legend_frame.pack(fill=tk.X, padx=6, pady=(8, 0))
        for val, color, label in [(0, ACCESS_COLORS[0], "0 - 阻挡"),
                                   (1, ACCESS_COLORS[1], "1 - 可通行"),
                                   (2, ACCESS_COLORS[2], "2 - 默认")]:
            row = ttk.Frame(legend_frame)
            row.pack(fill=tk.X, pady=1)
            c = tk.Canvas(row, width=14, height=14, highlightthickness=0)
            c.create_rectangle(1, 1, 13, 13, fill=color, outline="")
            c.pack(side=tk.LEFT, padx=(0, 6))
            ttk.Label(row, text=label).pack(side=tk.LEFT)

    def _build_toolbar(self, parent):
        toolbar = ttk.Frame(parent)
        toolbar.pack(fill=tk.X, pady=(0, 4))

        ttk.Button(toolbar, text="🖉 画笔", command=lambda: self._set_tool("brush")).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="🔍 取样", command=lambda: self._set_tool("picker")).pack(side=tk.LEFT, padx=2)
        ttk.Button(toolbar, text="▣ 填充", command=lambda: self._set_tool("fill")).pack(side=tk.LEFT, padx=2)

        self.tool_var = tk.StringVar(value="brush")
        self.tool_label = ttk.Label(toolbar, text="工具: 画笔")
        self.tool_label.pack(side=tk.LEFT, padx=8)

        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=8)

        ttk.Label(toolbar, text="缩放:").pack(side=tk.LEFT)
        ttk.Button(toolbar, text="−", width=2, command=lambda: self._zoom(-1)).pack(side=tk.LEFT)
        ttk.Button(toolbar, text="+", width=2, command=lambda: self._zoom(1)).pack(side=tk.LEFT)
        self.zoom_label = ttk.Label(toolbar, text=f"{self.cell_size}px")
        self.zoom_label.pack(side=tk.LEFT, padx=4)

        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=8)
        self.brush_preview = tk.Canvas(toolbar, width=24, height=24,
                                       highlightthickness=1, bg="#1a1a1a")
        self.brush_preview.pack(side=tk.LEFT, padx=4)
        ttk.Label(toolbar, text=f"T:{self.brush_values[0]} S:{self.brush_values[1]} "
                               f"E:{self.brush_values[2]} A:{self.brush_values[3]}").pack(side=tk.LEFT)

    # ------------------------------------------------------------------
    # 工具
    # ------------------------------------------------------------------
    def _set_tool(self, tool):
        self.tool_var.set(tool)
        names = {"brush": "画笔", "picker": "取样", "fill": "填充"}
        self.tool_label.config(text=f"工具: {names.get(tool, tool)}")

    def _set_brush(self, vals):
        self.brush_values = list(vals)
        self._update_brush_preview()

    def _update_brush_preview(self):
        self.brush_preview.delete("all")
        colors = [TERRAIN_COLORS.get(self.brush_values[0], "#888"),
                  STRUCT_COLORS.get(self.brush_values[1], "#888"),
                  ENTITY_COLORS.get(self.brush_values[2], "#888"),
                  ACCESS_COLORS.get(self.brush_values[3], "#888")]
        size = 11
        for i, c in enumerate(colors):
            x = (i % 2) * size + 1
            y = (i // 2) * size + 1
            self.brush_preview.create_rectangle(x, y, x + size, y + size, fill=c, outline="")

    # ------------------------------------------------------------------
    # 绘制
    # ------------------------------------------------------------------
    def _coord_to_cell(self, cx, cy):
        """画布坐标 → 格子坐标，无效返回 None"""
        x = int((cx + self.offset_x) // self.cell_size)
        y = int((cy + self.offset_y) // self.cell_size)
        if 0 <= x < self.width and 0 <= y < self.height:
            return x, y
        return None

    def _draw_all(self):
        self.canvas.delete("all")
        if self.width == 0 or self.height == 0:
            return

        cs = self.cell_size
        # 可见范围
        cw = self.canvas.winfo_width()
        ch = self.canvas.winfo_height()
        if cw < 10 or ch < 10:
            cw, ch = 800, 600

        x0 = max(0, self.offset_x // cs)
        y0 = max(0, self.offset_y // cs)
        x1 = min(self.width, (self.offset_x + cw) // cs + 2)
        y1 = min(self.height, (self.offset_y + ch) // cs + 2)

        mode = self.mode_var.get()

        for y in range(y0, y1):
            for x in range(x0, x1):
                cell = self.blocks[y][x]
                px = x * cs - self.offset_x
                py = y * cs - self.offset_y

                if mode == 0:  # 综合
                    tc = TERRAIN_COLORS.get(cell[0], "#666")
                    self.canvas.create_rectangle(px, py, px + cs, py + cs,
                                                 fill=tc, outline="#333333")
                    # Access 边框指示
                    if cell[3] == 0:
                        self.canvas.create_rectangle(px + 1, py + 1, px + cs - 1, py + cs - 1,
                                                     outline="#ff4444", width=2)
                    elif cell[3] == 1:
                        self.canvas.create_rectangle(px + 1, py + 1, px + cs - 1, py + cs - 1,
                                                     outline="#44ff44", width=1)
                    # STRuct 小方块 (右上角)
                    sc = STRUCT_COLORS.get(cell[1], "#555")
                    s = max(3, cs // 4)
                    self.canvas.create_rectangle(px + cs - s - 1, py + 1,
                                                 px + cs - 1, py + s + 1,
                                                 fill=sc, outline="")
                    # Entity 小方块 (左下角)
                    ec = ENTITY_COLORS.get(cell[2], "#555")
                    self.canvas.create_rectangle(px + 1, py + cs - s - 1,
                                                 px + s + 1, py + cs - 1,
                                                 fill=ec, outline="")
                else:
                    field_idx = mode - 1
                    palette = [TERRAIN_COLORS, STRUCT_COLORS, ENTITY_COLORS, ACCESS_COLORS][field_idx]
                    c = palette.get(cell[field_idx], "#666")
                    self.canvas.create_rectangle(px, py, px + cs, py + cs,
                                                 fill=c, outline="#333333")

                # 文字标注
                if cs >= 14:
                    if mode == 0:
                        text = str(cell[0])
                    else:
                        text = str(cell[mode - 1])
                    self.canvas.create_text(px + cs // 2, py + cs // 2,
                                            text=text, fill="#ffffff",
                                            font=("Consolas", max(6, cs // 3)))

        # 选中框
        if self.selected_cell:
            sx, sy = self.selected_cell
            spx = sx * cs - self.offset_x
            spy = sy * cs - self.offset_y
            self.canvas.create_rectangle(spx, spy, spx + cs, spy + cs,
                                         outline="#ffdd00", width=2)

        # 滚动区域
        self.canvas.configure(scrollregion=(0, 0, self.width * cs, self.height * cs))

    # ------------------------------------------------------------------
    # 事件处理
    # ------------------------------------------------------------------
    def _on_click(self, event):
        cell = self._coord_to_cell(event.x, event.y)
        if cell is None:
            return

        tool = self.tool_var.get()
        if tool == "brush":
            self._paint_cell(cell[0], cell[1])
            self._select_cell(cell[0], cell[1])
            self._draw_all()
        elif tool == "picker":
            self._pick_cell(cell[0], cell[1])
            self._select_cell(cell[0], cell[1])
            self._draw_all()
        elif tool == "fill":
            self._flood_fill(cell[0], cell[1])
            self._draw_all()

    def _on_drag(self, event):
        cell = self._coord_to_cell(event.x, event.y)
        if cell is None:
            return
        tool = self.tool_var.get()
        if tool == "brush":
            self._paint_cell(cell[0], cell[1])
            self._select_cell(cell[0], cell[1])
            self._draw_all()

    def _on_release(self, event):
        pass

    def _on_mousewheel(self, event):
        # 缩放
        if event.delta > 0:
            self._zoom(1)
        else:
            self._zoom(-1)

    def _on_middle_click(self, event):
        self._drag_start = (event.x, event.y, self.offset_x, self.offset_y)

    def _on_middle_drag(self, event):
        if hasattr(self, '_drag_start'):
            sx, sy, ox, oy = self._drag_start
            dx = event.x - sx
            dy = event.y - sy
            self.offset_x = max(0, ox - dx)
            self.offset_y = max(0, oy - dy)
            self._draw_all()

    def _on_mode_change(self):
        self._draw_all()

    def _on_field_change(self, idx):
        pass  # 值由 trace 直接更新

    # ------------------------------------------------------------------
    # 操作
    # ------------------------------------------------------------------
    def _paint_cell(self, x, y):
        self.blocks[y][x] = list(self.brush_values)
        self.modified = True
        self._update_title()

    def _pick_cell(self, x, y):
        self._set_brush(self.blocks[y][x])
        # 同步到属性面板
        for i in range(4):
            self.field_vars[i].set(self.blocks[y][x][i])

    def _flood_fill(self, x, y):
        """区域填充 — 替换连通区域中与起点值相同的格子"""
        target = tuple(self.blocks[y][x])
        replacement = list(self.brush_values)
        if target == tuple(replacement):
            return

        stack = [(x, y)]
        visited = set()
        while stack:
            cx, cy = stack.pop()
            if (cx, cy) in visited:
                continue
            if not (0 <= cx < self.width and 0 <= cy < self.height):
                continue
            if tuple(self.blocks[cy][cx]) != target:
                continue
            visited.add((cx, cy))
            self.blocks[cy][cx] = list(replacement)
            stack.extend([(cx + 1, cy), (cx - 1, cy), (cx, cy + 1), (cx, cy - 1)])
        self.modified = True
        self._update_title()
        self.status_label.config(text=f"填充: {len(visited)} 格")

    def _select_cell(self, x, y):
        self.selected_cell = (x, y)
        cell = self.blocks[y][x]
        self.sel_pos_label.config(text=f"({x}, {y})")
        for i in range(4):
            self.field_vars[i].set(cell[i])
        self.cell_label.config(
            text=f"({x},{y}) T:{cell[0]} S:{cell[1]} E:{cell[2]} A:{cell[3]}")

    def _apply_to_selected(self):
        if self.selected_cell is None:
            return
        x, y = self.selected_cell
        for i in range(4):
            self.blocks[y][x][i] = self.field_vars[i].get()
        self.modified = True
        self._update_title()
        self._draw_all()

    def _zoom(self, direction):
        old = self.cell_size
        self.cell_size = max(MIN_CELL, min(MAX_CELL, self.cell_size + direction * 2))
        # 保持视口中心
        if old != self.cell_size:
            ratio = self.cell_size / old
            self.offset_x = int(self.offset_x * ratio)
            self.offset_y = int(self.offset_y * ratio)
            self.zoom_label.config(text=f"{self.cell_size}px")
            self._draw_all()

    def _reset_view(self):
        self.offset_x = 0
        self.offset_y = 0
        self.cell_size = CELL_SIZE
        self.zoom_label.config(text=f"{self.cell_size}px")
        self._draw_all()

    def _fill_default(self):
        if not self.blocks:
            return
        if not messagebox.askyesno("确认", "将所有格子填充为默认值 (2,2,2,2)？"):
            return
        for row in self.blocks:
            for cell in row:
                cell[:] = [2, 2, 2, 2]
        self.modified = True
        self._update_title()
        self._draw_all()

    # ------------------------------------------------------------------
    # 文件操作
    # ------------------------------------------------------------------
    def _new_map(self):
        dialog = tk.Toplevel(self.root)
        dialog.title("新建地图")
        dialog.geometry("280x180")
        dialog.transient(self.root)
        dialog.grab_set()

        ttk.Label(dialog, text="宽度:").grid(row=0, column=0, padx=8, pady=6, sticky=tk.W)
        w_var = tk.IntVar(value=32)
        ttk.Spinbox(dialog, from_=1, to=9999, textvariable=w_var, width=10).grid(row=0, column=1)

        ttk.Label(dialog, text="高度:").grid(row=1, column=0, padx=8, pady=6, sticky=tk.W)
        h_var = tk.IntVar(value=32)
        ttk.Spinbox(dialog, from_=1, to=9999, textvariable=h_var, width=10).grid(row=1, column=1)

        ttk.Label(dialog, text="BlockSize:").grid(row=2, column=0, padx=8, pady=6, sticky=tk.W)
        bs_var = tk.IntVar(value=4)
        ttk.Spinbox(dialog, from_=1, to=16, textvariable=bs_var, width=10).grid(row=2, column=1)

        def do_create():
            w = w_var.get()
            h = h_var.get()
            if w <= 0 or h <= 0:
                return
            self.width = w
            self.height = h
            self.blockSize = bs_var.get()
            self.blocks = [[[2, 2, 2, 2] for _ in range(w)] for _ in range(h)]
            self.filepath = None
            self.modified = True
            self.selected_cell = None
            self.offset_x = 0
            self.offset_y = 0
            self.cell_size = CELL_SIZE
            self.info_label.config(text=f"{w} × {h}, blocksize={self.blockSize}")
            self._update_title()
            self._draw_all()
            dialog.destroy()

        ttk.Button(dialog, text="创建", command=do_create).grid(row=3, column=0, columnspan=2, pady=12)

    def _open_map(self):
        fp = filedialog.askopenfilename(
            title="打开 OCMP 地图",
            filetypes=[("OCMP 地图", "*.ocmp"), ("所有文件", "*.*")])
        if not fp:
            return
        try:
            w, h, blocks, bs, ver, lc = load_ocmp(fp)
            self.width = w
            self.height = h
            self.blocks = blocks
            self.blockSize = bs
            self.filepath = fp
            self.modified = False
            self.selected_cell = None
            self.offset_x = 0
            self.offset_y = 0
            self.cell_size = CELL_SIZE
            self.info_label.config(
                text=f"{w} × {h}, blocksize={bs}, ver={ver}, layers={lc}")
            self._update_title()
            self._draw_all()
            self.status_label.config(text=f"已打开: {os.path.basename(fp)}")
        except Exception as e:
            messagebox.showerror("打开失败", str(e))

    def _save_map(self):
        if self.filepath:
            try:
                save_ocmp(self.filepath, self.width, self.height,
                          self.blocks, self.blockSize)
                self.modified = False
                self._update_title()
                self.status_label.config(text=f"已保存: {os.path.basename(self.filepath)}")
            except Exception as e:
                messagebox.showerror("保存失败", str(e))
        else:
            self._save_as()

    def _save_as(self):
        fp = filedialog.asksaveasfilename(
            title="保存 OCMP 地图",
            defaultextension=".ocmp",
            filetypes=[("OCMP 地图", "*.ocmp"), ("所有文件", "*.*")])
        if not fp:
            return
        try:
            save_ocmp(fp, self.width, self.height, self.blocks, self.blockSize)
            self.filepath = fp
            self.modified = False
            self._update_title()
            self.status_label.config(text=f"已保存: {os.path.basename(fp)}")
        except Exception as e:
            messagebox.showerror("保存失败", str(e))

    def _update_title(self):
        name = os.path.basename(self.filepath) if self.filepath else "未命名"
        star = " *" if self.modified else ""
        self.root.title(f"OCMP Map Editor — {name}{star}")


# ---------------------------------------------------------------------------
# 入口
# ---------------------------------------------------------------------------
def main():
    root = tk.Tk()

    # 如果传了文件路径参数，启动后自动加载
    editor = MapEditor(root)

    if len(sys.argv) > 1 and os.path.exists(sys.argv[1]):
        fp = sys.argv[1]
        try:
            w, h, blocks, bs, ver, lc = load_ocmp(fp)
            editor.width = w
            editor.height = h
            editor.blocks = blocks
            editor.blockSize = bs
            editor.filepath = fp
            editor.info_label.config(
                text=f"{w} × {h}, blocksize={bs}, ver={ver}, layers={lc}")
            editor._update_title()
            editor._draw_all()
            editor.status_label.config(text=f"已加载: {os.path.basename(fp)}")
        except Exception as e:
            messagebox.showerror("加载失败", str(e))

    root.mainloop()


if __name__ == "__main__":
    main()
