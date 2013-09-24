#include "BattleAiAction.h"
#include "BattleUnit.h"
#include "BattleObj.h"

CBattleAiAction::~CBattleAiAction()
{
}

int CBattleAiAction::GetCurTick()
{
    return GetBattleObj()->m_iCurrTick;    
}

CBattleObj* CBattleAiAction::GetBattleObj()
{
    return m_holder->GetBattleObj();
}

CBattleAiControler* CBattleAiAction::GetAiControler()
{
    return &m_holder->m_aiControler;
}

bool CBattleAiAction::OnEnter()
{
    m_iEnterTick = GetCurTick();

    return true;    
}

void CBattleAiAction::SiblingDoAi()
{
    for(std::vector<CBattleAiActionList>::iterator iter = m_siblingActionListVec.begin(); iter != m_siblingActionListVec.end(); ++iter)
    {
        CBattleAiActionList &actionList = *iter;
        actionList.DoAi();
    }    
}

bool CBattleAiAction::OnLeave()
{
    m_iLeaveTick = GetCurTick();

    return true;    
}

CBattleAiStandAction::CBattleAiStandAction(CBattleUnit *pUnit) : CBattleAiAction(pUnit)
{
}

CBattleAiStandAction::~CBattleAiStandAction()
{
}

bool CBattleAiStandAction::CanDo()
{
    return true;    
}

bool CBattleAiStandAction::Doing()
{
    return true;    
}

bool CBattleAiStandAction::OnEnter()
{
    //进入站立行为
    CBattleAiAction::OnEnter();

    return true;
}

bool CBattleAiStandAction::OnLeave()
{
    //离开站立行为了
    CBattleAiAction::OnLeave();

    return true;    
}

CBattleAiFollowAction::CBattleAiFollowAction(CBattleUnit *pUnit) : CBattleAiAction(pUnit)
{
}

CBattleAiFollowAction::~CBattleAiFollowAction()
{
}

bool CBattleAiFollowAction::CanDo()
{    
    CBattleUnit *pFollowUnit = GetBattleObj()->GetUnit(GetAiControler()->FollowTargetId());

    if(pFollowUnit == NULL)
    {
        return false;
    }

    //检查距离，如果太近，不需要跟随
    int dis = CBattleUtils::GetDistance(m_holder->iPosX, m_holder->iPosY, pFollowUnit->iPosX, pFollowUnit->iPosY);

    //暂时写死吧, 不能位于5格内
    if(dis <= 5)
    {
        return false;
    }
    
    return true;    
}

bool CBattleAiFollowAction::Doing()
{
    if(m_holder->iCdTick > GetCurTick())
    {
        return false;
    }
    
    CBattleUnit *pFollowUnit = GetBattleObj()->GetUnit(GetAiControler()->FollowTargetId());    
    GetBattleObj()->MoveToTarget(m_holder, pFollowUnit->iPosX, pFollowUnit->iPosY, 5);
    
    return true;
}

bool CBattleAiFollowAction::OnEnter()
{
    CBattleAiAction::OnEnter();

    return true;    
}

bool CBattleAiFollowAction::OnLeave()
{
    CBattleAiAction::OnLeave();
    
    return true;    
}

CBattleAiChaseAction::CBattleAiChaseAction(CBattleUnit *pUnit) : CBattleAiAction(pUnit)
{
}

CBattleAiChaseAction::~CBattleAiChaseAction()
{
}

bool CBattleAiChaseAction::CanDo()
{
    if(GetAiControler()->AttackTargetId() == 0)
    {
        return false;
    }
    
    CBattleUnit *pChaseUnit = GetBattleObj()->GetUnit(GetAiControler()->AttackTargetId());

    if(pChaseUnit == NULL)
    {
        GetAiControler()->AttackTargetId(0);
        
        return false;
    }
    
    return true;    
}

bool CBattleAiChaseAction::Doing()
{
    if(m_holder->iCdTick > GetCurTick())
    {
        return false;
    }

    CBattleUnit *pChaseUnit = GetBattleObj()->GetUnit(GetAiControler()->AttackTargetId());
    GetBattleObj()->MoveToTarget(m_holder, pChaseUnit->iPosX, pChaseUnit->iPosY, GetBattleObj()->GetNormalAttackDis(m_holder, pChaseUnit));
    
    return true;
}

bool CBattleAiChaseAction::OnEnter()
{
    CBattleAiAction::OnEnter();

    return true;    
}

bool CBattleAiChaseAction::OnLeave()
{
    CBattleAiAction::OnLeave();
    
    return true;
}

CBattleAiAttackAction::CBattleAiAttackAction(CBattleUnit *pUnit) : CBattleAiAction(pUnit)
{
}

CBattleAiAttackAction::~CBattleAiAttackAction()
{
}

bool CBattleAiAttackAction::CanDo()
{
    if(GetAiControler()->AttackTargetId() == 0)
    {
        return false;
    }
    
    CBattleUnit *pAttackUnit = GetBattleObj()->GetUnit(GetAiControler()->AttackTargetId());
    
    if(pAttackUnit == NULL)
    {
        GetAiControler()->AttackTargetId(0);
        
        return false;
    }

    if(pAttackUnit->IsDead())
    {
        GetAiControler()->AttackTargetId(0);

        return false;
    }
    
    int dis = CBattleUtils::GetDistance(m_holder->iPosX, m_holder->iPosY, pAttackUnit->iPosX, pAttackUnit->iPosY);
    
    if(dis > GetBattleObj()->GetNormalAttackDis(m_holder, pAttackUnit))
    {
        return false;
    }
    
    return true;
}

bool CBattleAiAttackAction::Doing()
{    
    if(m_holder->m_state == battle_unit_state::STATE_MOVE)
    {
        m_holder->MoveLastPos();
        m_holder->iCdTick = GetCurTick() + CONFIG_DATA->m_iServerFPS * battle::PIXEL_INTERVAL / m_holder->GetSpeed();
        
        return false;        
    }
    
    if(m_holder->iCdTick > GetCurTick())
    {
        return false;
    }
    
    CBattleUnit *pAttackUnit = GetBattleObj()->GetUnit(GetAiControler()->AttackTargetId());    
    GetBattleObj()->AttackTarget(m_holder, pAttackUnit);
    
    return true;
}

bool CBattleAiAttackAction::OnEnter()
{
    CBattleAiAction::OnEnter();

    return true;    
}

bool CBattleAiAttackAction::OnLeave()
{
    CBattleAiAction::OnLeave();

    return true;    
}

CBattleAiControler::CBattleAiControler(CBattleUnit *holder)
{
    m_holder = holder;
    m_iFollowTargetId = 0;
    m_iAttackTargetId = 0;
    m_bEnableAi = false;    
    m_iDoAiCdTick = 0;
    BuildAction();
}

int CBattleAiControler::GetCurTick()
{
    return m_holder->GetBattleObj()->m_iCurrTick;
}

CBattleAiControler::~CBattleAiControler()
{
    DeleteAction();    
}

void CBattleAiControler::DoAi()
{
    if(m_holder->IsDead())
    {
        return;
    }
    
    m_holder->m_moveControler.DoMove();

    //是否开启ai控制
    if(!EnableAi())
    {
        return;
    }

    // //进入战斗后延迟一会执行ai, 因为前台加载资源需要时间，否则导致角色瞬移的问题
    // if(GetCurTick() < CONFIG_DATA->m_iServerFPS * 2)
    // {
    //     return;        
    // }
    
    if(AttackTargetId() == 0)
    {
        int iTargetId = 0;
        
        if(GetCurTick() < m_iDoAiCdTick)
        {
            return;
        }

        m_iDoAiCdTick = GetCurTick() + 30;
        
        if(m_holder->IsPriHero())
        {
            //主将的话，查找所有的目标
            iTargetId = m_holder->FindTargetInAll();            
        }
        else
        {
            iTargetId = m_holder->FindTargetInRange(20);
        }
        
        AttackTargetId(iTargetId);        
    }

    m_firstAiActionList.DoAi();
}

void CBattleAiActionList::DoAi()
{
    for(std::vector<CBattleAiAction*>::iterator iter = m_actionVec.begin(); iter != m_actionVec.end(); ++iter)
    {
        CBattleAiAction *pAction = *iter;

        if(pAction->CanDo())
        {
            if(pAction != m_pCurAiAction)
            {
                if(m_pCurAiAction != NULL)
                {
                    m_pCurAiAction->OnLeave();
                }

                pAction->OnEnter();
            }

            m_pCurAiAction = pAction;
            m_pCurAiAction->Doing();
            m_pCurAiAction->SiblingDoAi();

            break;            
        }
    }    
}

void CBattleAiControler::BuildAction()
{
    CBattleAiAction* actionList[] = {
        new CBattleAiAttackAction(m_holder),
        new CBattleAiChaseAction(m_holder),
        new CBattleAiFollowAction(m_holder),
        new CBattleAiStandAction(m_holder)
    };

    const int iCount = sizeof(actionList) / sizeof(actionList[0]);

    for(int i = 0; i != iCount; ++i)
    {
        CBattleAiAction *pAction = actionList[i];
        m_firstAiActionList.AddAction(pAction);
        AddAction(pAction);        
    }
}

void CBattleAiControler::AddAction(CBattleAiAction *pAction)
{
    m_allActionSet.insert(pAction);
}

void CBattleAiControler::DeleteAction()
{
    for(std::set<CBattleAiAction*>::iterator iter = m_allActionSet.begin(); iter != m_allActionSet.end(); ++iter)
    {
        delete *iter;        
    }    
}
