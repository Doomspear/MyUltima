#include "StdAfx.h"
#include "Dragon.h"
#include "Includes.h"

CDragon::CDragon(void)
{
	m_pt.x			= 5;
	m_pt.y			= 5;
}

CDragon::CDragon(_INT ix, _INT iy)
{
	m_pt.x			= ix;
	m_pt.y			= iy;
}

CDragon::~CDragon(void)
{
	//Release();
}

void CDragon::Init()
{	
	//from CObj
	m_pvecTex		= _GETINST( CTextureLoader )->GetTextureVector();
	D3DXMatrixIdentity( &m_matWorld );									//m_matWorld setting
	
	m_vCenter		= D3DXVECTOR3( sqrtf(m_fSizeX), m_fSizeY, 0.0f );
	
	MoveToIndex();														//m_vPos setting

	m_vLook			= D3DXVECTOR3( 1.0f, 0.0f, 0.0f );

	D3DXMatrixScaling( &m_matScale, 1.0f, 1.0f, 0.0f );					//m_matScale setting
	D3DXMatrixTranslation( &m_matTrans, m_vPos.x, m_vPos.y, 0.0f );		//m_matTras setting
	
	m_pCloth		= NULL;
	m_pWeapon		= NULL;

	m_pTarget		= NULL;

	m_pInven		= new INVENTORY( this );
	m_pInven		-> Init();
	
	m_dwTexID		= TI_DRAGON;
	m_dwMidID		= MI_STAND;
	m_dwMidIDPast	= m_dwMidID;
	m_dwDir			= DI_DOWNLEFT;
	
	SetTileIndex();														//m_iTileIndex setting
	//m_iDepth;

	m_dwState		= ST_STAND | ST_PEACE;

	m_iFrame		= 0;
	m_bReverse		= false;
	m_bDecay		= false;
	
	m_dwTime		= GetTickCount();
	m_dwTick		= 120;
	
	m_fSizeX		= 100.0f;
	m_fSizeY		= 100.0f;
	
	m_fSpeed		= 150.0f;		//m_fSpeed		= 2.0f;
	m_fDegree		= 45.0f;
	
	m_iLocomotive	= 3;
	
	//for char stat
	m_iAction		= 0;

	//m_iHP			= 50;
	m_iHP			= 300;
	m_iMP			= 10;	
	
	m_iSTR			= 50;
	m_iDEX			= 50;
	m_iINT			= 10;

	m_iATT			= 15;
	m_iAR			= 0;	

	m_iWeight		= 0;

	m_ptPast		= m_pt;
	SetColRect();														//m_rcColi setting;
 

	//from CCharacter
	
	//m_vecInven;
	//m_listBestTile;
	ZeroMemory( &m_vDest, sizeof(m_vDest) );	//m_vDest setting
	//m_ptDest;		
	m_iSelect		= 255;						//선택됐을때의 컬러

	m_dwMoveTime	= GetTickCount();
	m_dwAttTime		= GetTickCount();
	m_dwAstarTime	= GetTickCount();
	m_bMove			= false;
	m_bArmed		= true;
	m_bDead			= false;
	m_bAttMotion	= false;

	m_bPressed[BI_END];
	
	//private member
	m_bClicked		= false;
}

void CDragon::Progress()
{	
	CheckReverse();

	MoveToIndex();
	RevisionPos();

	D3DXMatrixTranslation( &m_matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	m_matWorld = m_matScale * m_matTrans;
	
	if ( m_iHP <= 0 )
	{
		m_dwMidID	= MI_DIE;
		
		MoveToIndex();
		RevisionPos();
		if ( m_iFrame == (*m_pvecTex)[m_dwTexID][m_dwMidID][m_dwDir].size() - 1 )
		{
			Die();
		}
		return;
	}

	if ( m_dwMoveTime + m_dwTick < GetTickCount() )
	{
		m_dwMoveTime = GetTickCount();
		BestMove();
		MoveToDest();
	}

	CheckState();
	SetColRect();
	SelectCheck();
	
	Attack();
}

void CDragon::Render()
{
	FrameMove();

	m_vCenter = (*m_pvecTex)[m_dwTexID][m_dwMidID][m_dwDir][m_iFrame]->vCen;

	D3DXMatrixRotationZ( &m_matRotateZ, D3DXToRadian( 15 ) );
	
	m_matWorld = m_matScale * m_matRotateZ * m_matTrans;
	_GETINST( CDevice )->GetSprite()->SetTransform( &m_matWorld );
	_GETINST( CDevice )->GetSprite()->Draw( (*m_pvecTex)[m_dwTexID][m_dwMidID][m_dwDir][m_iFrame]->pTex, NULL, &m_vCenter, NULL, D3DCOLOR_ARGB(100, 0, 0, 0) );
	
	m_matWorld = m_matScale * m_matTrans;
	_GETINST( CDevice )->GetSprite()->SetTransform( &m_matWorld );
	_GETINST( CDevice )->GetSprite()->Draw( (*m_pvecTex)[m_dwTexID][m_dwMidID][m_dwDir][m_iFrame]->pTex, NULL, &m_vCenter, NULL, D3DCOLOR_ARGB(255, 255, m_iSelect, m_iSelect) );
		
	//DrawColiBox();
	DrawHP();
}

void CDragon::Release()
{
	/*delete m_pInven;
	m_pInven = NULL;*/
}