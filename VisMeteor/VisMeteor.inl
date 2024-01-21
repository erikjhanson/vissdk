// VisMeteor.inl
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved


inline bool CVisMeteor::CheckScales(void)
{
	if (m_fPreserveAspect)
	{
		// UNDONE:  We should include the offset in these equations.
		double dblScaleX = ((double) m_size.cx) / (m_sizeBaseline.cx);
		double dblScaleY = ((double) m_size.cy) / (m_sizeBaseline.cy);
		double dblScale = max(dblScaleX, dblScaleY);

		if (dblScale != m_dblScale)
			MdigControl(m_milidDig, M_GRAB_SCALE, dblScale);

		m_dblScale = m_dblScaleX = m_dblScaleY = dblScale;

		return TRUE;
	} else {
		// For Meteor II, subsampling must be  between 1 and 16
		double dblScaleX = ((double) m_size.cx) / (m_sizeBaseline.cx);
		double dblScaleY = ((double) m_size.cy) / (m_sizeBaseline.cy);

		// Scale X
		if ((1.0/dblScaleX < 1.0) || (1.0/dblScaleX > 16.0) ||		// Check range 1..16
			(m_sizeBaseline.cx % m_size.cx != 0)) {					// Check integer scaling
			return FALSE;
		} else if (dblScaleX != m_dblScaleX) {
			MdigControl(m_milidDig, M_GRAB_SCALE_X, dblScaleX);
			m_dblScaleX = dblScaleX;
		}
	
		if ((1.0/dblScaleY < 1.0) || (1.0/dblScaleY > 16.0) ||		// Check range 1..16
			(m_sizeBaseline.cy % m_size.cy != 0)) {					// Check integer scaling
			return FALSE;
		} else if (dblScaleY != m_dblScaleY) {
			MdigControl(m_milidDig, M_GRAB_SCALE_Y, dblScaleY);
			m_dblScaleY = dblScaleY;
		}

		return TRUE;
	}
}


inline void CVisMeteor::CheckFrameOrFieldGrab(bool fInit)
{
	// Grab fields if width and height are less than half baseline.
	// Note that is seems that the last condition should not be needed,
	// but it appears to be needed in practice.
	bool fGrabFields;
	if ((m_size.cx <= m_sizeBaseline.cx / 2)
			&& (((m_sizeBaseline.cy / 2 / m_size.cy) * 2 * m_size.cy)
					== m_sizeBaseline.cy)
			&& ((m_size.cy & 3) == 0))
	{
		fGrabFields = true;
	}
	else
	{
		fGrabFields = false;
	}

	if ((fInit) || (fGrabFields != m_fGrabFields))
	{
		if (fGrabFields)
		{
			MdigControl(m_milidDig, M_GRAB_FIELD_NUM, 1.0);
		}
		else
		{
			MdigControl(m_milidDig, M_GRAB_FRAME_NUM, 1.0);
		}

		m_fGrabFields = fGrabFields;
	}
}
