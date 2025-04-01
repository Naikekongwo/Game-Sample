#include "map.h"

recmap::recmap(int x = 0, int y = 0, int h = 0, int w = 0, int l = 0, int tk = 0) : recarea(x, y, h, w), layer(l), timekeeper(tk), time(0), turn(0), prg(new std::vector<recgraphic>) {};
int recmap::get_layer() const { return layer; }
int recmap::set_layer(int l)
{
    if (l >= 0)
    {
        layer = l;
        return 1;
    }
    return 0;
}
recmap::~recmap()
{
    delete prg;
};
int recmap::load(int t, SDL_Surface *surf, mywin mwin)
{
    auto s = prg->begin();
    s = s + t;
    s->load(surf, mwin);
    time++;
    if (time == timekeeper)
    {
        turn = set_turn(((turn + 1) < (prg->end() - prg->begin())) ? (turn + 1) : 0);
        time = 0;
    }
}
int recmap::get_turn() const { return turn; }
int recmap::set_turn(int t)
{
    turn = t;
    return t;
}
int recmap::time_run()
{
    time++;
    return time;
}
int recmap::reset_time()
{
    time = 0;
    return time;
}
std::vector<recgraphic> *recmap::get_prg() const { return prg; }
int recmap::add_recg(recgraphic rg)
{
    get_prg()->push_back(rg);
    return 1;
}
int recmap::get_timekeeper() const { return timekeeper; }
int recmap::set_timekeeper(int tk)
{
    timekeeper = tk;
    return tk;
}
recgraphic::recgraphic(int x, int y, int h, int w, const std::string &s) : selectedx(x), selectedy(y), sysheight(h), syswidth(w), gurl(s) {};
int recgraphic::set_gurl(const std::string &s)
{
    gurl = s;
    return 1;
}
int recgraphic::set_selectedx(int sx)
{
    selectedx = sx;
    return sx;
}
int recgraphic::set_selectedy(int sy)
{
    selectedy = sy;
    return sy;
}
const std::string &recgraphic::get_gurl() const { return gurl; }
int recgraphic::get_selectedx() const { return selectedx; }
int recgraphic::get_selectedy() const { return selectedy; }
int recgraphic::load(SDL_Surface *surf, mywin mwin)
{
    SDL_Surface *bmp_surf = SDL_LoadBMP(gurl.c_str());
    SDL_Rect a = {selectedx, selectedy, syswidth, sysheight};
    SDL_Rect b = {mwin.get_rtx(), mwin.get_rty(), syswidth, sysheight};
    SDL_BlitSurface(bmp_surf, &a, surf, &b);
    SDL_FreeSurface(bmp_surf);
}
recmapset_top::recmapset_top(int sh, int sw, int mh, int mw, int ml) : sysheight(sh), syswidth(sw), maxh(mh), maxw(mw), maxlayer(ml), mapset(nullptr) {};
recmapset_top::~recmapset_top() { delete[] mapset; }
int recmapset_top::get_sysheight() const { return sysheight; }
int recmapset_top::get_syswidth() const { return syswidth; }
int recmapset_top::get_maxh() const { return maxh; }
int recmapset_top::get_maxw() const { return maxw; }
int recmapset_top::get_maxlayer() const { return maxlayer; }
recmap *recmapset_top::get_mapset() const { return mapset; }
int recmapset_top::set_mapset()
{
    mapset = new recmap[get_maxh() * get_maxw()];
    for (int i = 0; i < get_maxh() * get_maxw(); i++)
    {
        mapset[i].set_height(sysheight);
        mapset[i].set_width(syswidth);
        mapset[i].set_stpx((i % maxw) * syswidth);
        mapset[i].set_stpy((i / maxw) * sysheight);
    }
    return 1;
}
int recmapset_top::load(mywin mw, SDL_Surface *surf, SDL_Window *w, mywin mwin)
{
    for (int k = 0; k <= maxlayer; k++)
    {
        for (int i = ((mw.get_rty() - 5 * sysheight) / sysheight >= 0 ? ((mw.get_rty() - 5 * sysheight) / sysheight) : 0); i <= ((mw.get_rty() + mw.get_winheight() + 5 * sysheight) / sysheight <= maxh ? ((mw.get_rty() + mw.get_winheight() + 5 * sysheight) / sysheight) : maxh); i++)
        {
            for (int j = ((mw.get_rtx() - 5 * syswidth) / syswidth >= 0 ? ((mw.get_rtx() - 5 * syswidth) / syswidth) : 0); j <= ((mw.get_rtx() + mw.get_winwidth() + 5 * syswidth) / syswidth <= maxw ? ((mw.get_rtx() + mw.get_winwidth() + 5 * syswidth) / syswidth) : maxw); j++)
            {
                if (!mapset[i * maxw + j].get_prg()->empty() && mapset[i * maxw + j].get_layer() == k)
                {
                    mapset[i * maxw + j].load(mapset[i * maxw + j].get_turn(), surf, mwin);
                }
            }
        }
    }
    SDL_UpdateWindowSurface(w);
}
mywin::mywin(int x, int y, int wh, int ww) : rtx(x), rty(y), winheight(wh), winwidth(ww) {};
int mywin::get_rtx() const { return rtx; }
int mywin::get_rty() const { return rty; }
int mywin::get_winheight() const { return winheight; }
int mywin::get_winwidth() const { return winwidth; };
recmapset_top read_map(const std::string &murl)
{
    std::ifstream inputFile(murl.c_str());
    std::string word;
    std::string url;
    int arr[8];
    for (int i = 0; i < 5; i++)
    {
        inputFile >> word;
        arr[i] = std::stoi(word);
    }
    recmapset_top a(arr[0], arr[1], arr[2], arr[3], arr[4]);
    a.set_mapset();
    while (inputFile >> word)
    {
        arr[0] = std::stoi(word);
        for (int i = 0; i < 4; i++)
        {
            inputFile >> word;
            arr[i + 1] = std::stoi(word);
        }
        inputFile >> word;
        url = word;
        a.get_mapset()[arr[0]].add_recg(recgraphic(arr[3], arr[4], a.get_sysheight(), a.get_syswidth(), url));
        a.get_mapset()[arr[0]].set_layer(arr[1]);
        a.get_mapset()[arr[0]].set_timekeeper(arr[2]);
    }
    inputFile.close();
    return a;
}
int write_map(const std::string &murl, recmapset_top rt)
{
    std::ofstream outputFile(murl.c_str());
    outputFile << rt.get_sysheight() << " " << rt.get_syswidth() << " " << rt.get_maxh() << " " << rt.get_maxw() << " " << rt.get_maxlayer() << std::endl;
    for (int i = 0; i < rt.get_maxh() * rt.get_maxw(); i++)
    {
        for (recgraphic s : *(rt.get_mapset()[i].get_prg()))
        {
            outputFile << i << " " << rt.get_mapset()[i].get_layer() << " " << rt.get_mapset()[i].get_timekeeper() << " " << s.get_selectedx() << " " << s.get_selectedy() << " " << s.get_gurl() << std::endl;
        }
    }
    outputFile.close();
    return 1;
}