/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      _____       ___   _____   _____    _   _____   _               *
 *     /  ___|     /   | |  _  \ |  _  \  | | | ____| | |              *
 *     | |        / /| | | |_| | | |_| |  | | | |__   | |              *
 *     | |  _    / / | | |  _  { |  _  /  | | |  __|  | |              *
 *     | |_| |  / /  | | | |_| | | | \ \  | | | |___  | |___           *
 *     \_____/ /_/   |_| |_____/ |_|  \_\ |_| |_____| |_____|          *
 *                                                                     *
 *     gabriel is an angel from the Holy Bible, this engine is         *
 *   named gabriel, standing for bring good news to people. the        *
 *   goal of gabriel server engine is to help people to develop        *
 *   various online games, welcome you to join in.                     *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @github: https://github.com/lichuan/gabriel                       *
 *   @date: 2013-11-28 21:30:49                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BATTLE_UNIT_ACTION__
#define __BATTLE_UNIT_ACTION__

#include "BattleCommon.h"

class CBattleUnit;
class CBattleObj;
class CBattleAiControler;
class CBattleAiAction;

//ai后续行为链
class CBattleAiActionList
{
public:
    CBattleAiActionList()
    {
        m_pCurAiAction = NULL;
    }
    
    void AddAction(CBattleAiAction *pAction)
    {
        m_actionVec.push_back(pAction);
    }
    
    void DoAi();
    
private:    
    CBattleAiAction *m_pCurAiAction;
    std::vector<CBattleAiAction*> m_actionVec;
};

//ai行为基类
class CBattleAiAction
{
public:
    CBattleAiAction(CBattleUnit *pUnit)
    {
        m_iEnterTick = 0;
        m_iLeaveTick = 0;
        m_holder = pUnit;
    }
    
    virtual ~CBattleAiAction();    
    virtual bool CanDo() = 0;    
    virtual bool Doing() = 0;    
    virtual bool OnEnter();    
    virtual bool OnLeave();
    void SiblingDoAi();

    void AddSiblingActionList(CBattleAiActionList &actionList)
    {
        m_siblingActionListVec.push_back(actionList);
    }
    
protected:
    int GetCurTick();
    CBattleObj* GetBattleObj();
    CBattleAiControler* GetAiControler();
    int m_iEnterTick;
    int m_iLeaveTick;
    CBattleUnit *m_holder;

private:
    void SiblingOnEnter();
    void SiblingOnLeave();
    void SiblingDoing();
    std::vector<CBattleAiActionList> m_siblingActionListVec; //平级兄弟ai行为，当前的行为CanDo时，会执行兄弟行为。
};

class CBattleAiStandAction : public CBattleAiAction
{
public:
    CBattleAiStandAction(CBattleUnit *pUnit);
    virtual ~CBattleAiStandAction();
    
    virtual bool CanDo();
    virtual bool Doing();
    virtual bool OnEnter();
    virtual bool OnLeave();    
};

//跟随行为
class CBattleAiFollowAction : public CBattleAiAction
{
public:
    CBattleAiFollowAction(CBattleUnit *pUnit);
    virtual ~CBattleAiFollowAction();
    virtual bool CanDo();
    virtual bool Doing();
    
private:
    CBattleUnit *m_pFollowUnit;    
};

//返回组中心点
class CBattleAiGoBackAction : public CBattleAiAction
{
public:
    CBattleAiGoBackAction(CBattleUnit *pUnit);
    virtual ~CBattleAiGoBackAction();
    virtual bool CanDo();
    virtual bool Doing();
};

//追击行为
class CBattleAiChaseAction : public CBattleAiAction
{
public:
    CBattleAiChaseAction(CBattleUnit *pUnit);
    virtual ~CBattleAiChaseAction();
    virtual bool CanDo();
    virtual bool Doing();

private:
    CBattleUnit *m_pChaseUnit;    
};

//攻击行为
class CBattleAiAttackAction : public CBattleAiAction
{
public:
    CBattleAiAttackAction(CBattleUnit *pUnit);
    virtual ~CBattleAiAttackAction();
    virtual bool CanDo();
    virtual bool Doing();
    virtual bool OnEnter();
    
private:
    CBattleUnit *m_pAttackUnit;    
};

//定时出兵
class CBattleAiCreateUnitAction : public CBattleAiAction
{
public:
    CBattleAiCreateUnitAction(CBattleUnit *pUnit);
    virtual ~CBattleAiCreateUnitAction();
    virtual bool CanDo();
    virtual bool Doing();
};

//陷坑爆炸
class CBattleAiBurstAction : public CBattleAiAction
{
public:
    CBattleAiBurstAction(CBattleUnit *pUnit);
    virtual ~CBattleAiBurstAction();
    virtual bool CanDo();
    virtual bool Doing();
};

//靠近变身
class CBattleAiChangeAction : public CBattleAiAction
{
public:
    CBattleAiChangeAction(CBattleUnit *pUnit);
    virtual ~CBattleAiChangeAction();
    virtual bool CanDo();
    virtual bool Doing();
};

//ai 控制器
class CBattleAiControler
{
public:
    CBattleAiControler(CBattleUnit *holder);
    ~CBattleAiControler();

    void DoAi();
    
    //分配行为
    void BuildAiAction(int AiId = 0);

    //组装ai
    void BuildAi_0();    
    void BuildAi_1();
    void BuildAi_2();
    void BuildAi_3();
    void BuildAi_4();
    
    //释放行为
    void DeleteAction();
    
    int FollowTargetId()
    {
        return m_iFollowTargetId;
    }

    void FollowTargetId(int id)
    {
        m_iFollowTargetId = id;
    }

    void AddAction(CBattleAiAction *pAction);

    int AttackTargetId()
    {
        return m_iAttackTargetId;
    }

    void AttackTargetId(int id)
    {
        m_iAttackTargetId = id;
    }

    bool EnableAi()
    {
        return m_bEnableAi;
    }

    void EnableAi(bool enable)
    {
        m_bEnableAi = enable;
    }

    int GetCurTick();
    int m_iCreateTick; //创建兵cd
    
private:
    void RegBuildFuncs();
    CBattleUnit *m_holder;
    CBattleAiActionList m_firstAiActionList; //第一条ai行为链
    std::set<CBattleAiAction*> m_allActionSet;
    void (CBattleAiControler::*m_funcArr[battle_ai::MAX])();
    CBattleAiActionList BuildActionList(CBattleAiAction **pActionArr, int iArrLen);    
    int m_iFollowTargetId;
    int m_iAttackTargetId;
    bool m_bEnableAi;
    int m_iDoAiCdTick;
};

#endif
