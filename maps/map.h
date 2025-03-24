#ifndef _MAP_H_
#define _MAP_H_

#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class recarea{
    public:
        recarea(int h,int w):height(h),width(w),stpx(-1),stpy(-1){};
        recarea(int x,int y,int h,int w):height(h),width(w),stpx(x),stpy(y){};
        int get_stpx()const{return this->stpx;}
        int get_stpy()const{return this->stpy;}
        int get_height()const{return this->height;}
        int get_width()const{return this->width;}
        int set_stpx(int x){stpx=x;}
        int set_stpy(int y){stpy=y;}
        int set_height(int h){height=h;}
        int set_width(int w){width=w;}
    private:
        int stpx,stpy;
        int height,width;
        
};
class recmap:public recarea{
    public:
        recmap(int x,int y,int h,int w,int l,int tk);
        int get_layer()const;
        int set_layer(int l);
        ~recmap();
        int load(int t,SDL_Surface *surf,mywin mwin);
        int get_turn()const;
        int set_turn(int t);
        int time_run();
        int reset_time();
        int add_recg(recgraphic rg);
        int get_timekeeper()const;
        int set_timekeeper(int tk);
        std::vector<recgraphic>* get_prg()const;
    private:
        int layer;
        std::vector<recgraphic> *prg;
        int timekeeper;
        int time;
        int turn;
};
class recgraphic{
    public:
        recgraphic(int x,int y,int h,int w,const std::string &s);
        int set_gurl(const std::string &s);
        int set_selectedx(int sx);
        int set_selectedy(int sy);
        const std::string& get_gurl()const;
        int get_selectedx()const;
        int get_selectedy()const;
        int load(SDL_Surface* surf,mywin mwin);
    private:
        std::string gurl;
        int selectedx;
        int selectedy;
        int sysheight;
        int syswidth;

};
class recmapset_top{
    public:
        recmapset_top(int sh,int sw,int mh,int mw,int ml);
        ~recmapset_top();
        int get_sysheight()const;
        int get_syswidth()const;
        int get_maxh()const;
        int get_maxw()const;
        int get_maxlayer()const;
        recmap* get_mapset()const;
        int set_mapset();
        int load(mywin mw,SDL_Surface *surf,SDL_Window *w,mywin mwin);
    private:
        recmap *mapset;
        int sysheight;
        int syswidth;
        int maxh;
        int maxw;
        int maxlayer;

};
class mywin{
    public:
        mywin(int x,int y,int wh,int ww);
        int get_rtx()const;
        int get_rty()const;
        int get_winheight()const;
        int get_winwidth()const;
    private:
        int rtx;
        int rty;
        int winheight;
        int winwidth;
};
recmapset_top read_map(const std::string &murl);
int write_map(const std::string &murl,recmapset_top rt);
#endif