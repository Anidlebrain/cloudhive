﻿#include "puppeteer.h"

Puppeteer::Puppeteer(Game* G) : Monster(G) {
    id = Game::PUPPETEER;
    name = "系命傀儡师"; shortname = "傀儡师";
    initialhealth = 1666;
    sham = 0;
    icon = QString("res/icon/monster/") + "7_puppeteer.jpg";
}

void Puppeteer::reset() {
    sham = 0;
    prev_nlines = 0;
    Game::Monster::reset();
}


QString Puppeteer::description() {
    QString str = "磷酸设计："+ this->name + "\n初始血量:" + QN(initialhealth) + "\n";
    str += "技能0：【傀儡军团】从第三回合开始，每回合开始时分数增加2分\n\n";
    str += "技能1：【暗器·飞针】回合开始时，对玩家造成2点伤害，若分数高于玩家则额外造成1点伤害\n\n";
    str += "技能2：【命运之丝】战斗开始前，玩家每连成一线，分数临时增加7分\n\n";
    str += "技能3：【机关游行】每十回合发动一次，回合开始时，立即夺取后续的3张卡牌，获得卡牌数字之和的护盾值，并在护盾击碎之前暂时获得等量的分数\n\n";
    str += "副本环境：千丝机关阵：含有1、2、3数字的卡牌变成3张\n";
    str += "注意: 尚未确认此副本平衡性，请谨慎参考初始血量\n";
    return str;
}


// 回合开始前
void Puppeteer::Monster_Before_Turn() {
    if (G->turn >= 3) {
        this->addPoint(2);
        emit G->Alert_monster(name+"【傀儡军团】分数+2");
    }
    {
        int start_turn_damage=2;
        if(point > G->player->point())
        {
            start_turn_damage=3;
        }
        emit G->Alert_monster(name+"【暗器·飞针】触发，造成"+QN(start_turn_damage)+"点伤害!");

        G->player->take_damage(start_turn_damage);
    }
    if (G->turn % 10 == 0) {

        int armor_gain=0;
        QString notice = name + "【机关游行】发动，夺取";

        for(int i=0;i<G->turn/10+1;i++)
        {
            Game::Piece t=this->G->pool->drawout();
            armor_gain+=t.sum();
            notice=notice+" "+t.to_string();
        }
        sham = armor_gain;
        emit G->Alert_monster( notice + "\n"
                              + name + "获得" + QN(armor_gain) + "护盾和临时分数");

        this->addPoint(armor_gain);
        this->gainArmor(armor_gain);

        }
    Game::Monster::Monster_Before_Turn();
}

void Puppeteer::Monster_Before_Combat() {
    Game::Monster::Monster_Before_Combat();

    {
        int diff_nlines=G->player->grid.nlines()-prev_nlines;
        if(diff_nlines>0)
        {
        int pt_gain=diff_nlines*7;
        this->addPoint(pt_gain);
        emit G->Alert_monster(name+"【命运之丝】触发，分数+"+QN(pt_gain));
        }
        else if(diff_nlines<0)
        {
        int pt_lose=diff_nlines*(-7);
        this->addPoint(-pt_lose);
        emit G->Alert_monster(name+"【命运之丝】触发，分数-"+QN(pt_lose));
        }
    }
}


void Puppeteer::Monster_Combat() {
//    Game::Grid *gr = &G->player->grid;
//    int nl = gr->nlines();
//    this->addPoint(7*nl);
    Game::Monster::Monster_Combat();
//    this->addPoint(-7*nl);
}

void Puppeteer::Monster_After_Combat() {
    Game::Monster::Monster_After_Combat();

    prev_nlines=G->player->grid.nlines();

    if (this->armor <= 0 && sham>0 ) {
        this->addPoint(-1*sham);
        emit G->Alert_monster(name+"护盾被击穿，失去【机关游行】临时分数"+QN(sham)+"点!");
        sham = 0;
    }
}
