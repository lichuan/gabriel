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
        m_pCurAction = NULL;
    }
    
    void AddAction(CBattleAiAction *pAction)
    {
        m_actionVec.push_back(pAction);
    }
    
    void SetCurAction(CBattleAiAction *pAction)
    {
        m_pCurAction = pAction;
    }

    void DoAi();    
    
private:    
    CBattleAiAction *m_pCurAction;
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

//站立行为
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
    virtual bool OnEnter();
    virtual bool OnLeave();
};

//追击行为
class CBattleAiChaseAction : public CBattleAiAction
{
public:
    CBattleAiChaseAction(CBattleUnit *pUnit);
    virtual ~CBattleAiChaseAction();

    virtual bool CanDo();
    virtual bool Doing();
    virtual bool OnEnter();
    virtual bool OnLeave();
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
    virtual bool OnLeave();
};
    
//ai 控制器
class CBattleAiControler
{
public:
    CBattleAiControler(CBattleUnit *holder);
    ~CBattleAiControler();

    void DoAi();
    
    //分配行为
    void BuildAction();
    
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
    
private:
    CBattleUnit *m_holder;
    CBattleAiActionList m_firstAiActionList; //第一条ai行为链
    std::set<CBattleAiAction*> m_allActionSet;
    int m_iFollowTargetId;
    int m_iAttackTargetId;
    bool m_bEnableAi;
    int m_iDoAiCdTick;    
};

#endif
