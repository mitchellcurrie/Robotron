//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: inputlinebuffer.h
// Description	: header file
// Author		: Mitchell Currie
// Mail			: Mitchell.Currie@mediadesign.school.nz
//
#pragma once

#ifndef __INPUTLINEBUFFER_H__
#define __INPUTLINEBUFFER_H__

class CInputLineBuffer
{
public:
	CInputLineBuffer(unsigned int uBufferSize);
	~CInputLineBuffer();
	
	void ClearString();
	bool Update();
	void PrintToScreenTop();
	const char* GetString() const;

protected:
	unsigned int	m_uBufferSize;	//!< The total size of the buffer.
	unsigned int	m_uBufferPos;	//!< The position of the next char in the buffer to be entered by the user.
	char*			m_pBuffer;		//!< The buffer contents.
};

#endif