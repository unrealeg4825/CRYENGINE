// Copyright 2001-2018 Crytek GmbH / Crytek Group. All rights reserved.

#include "StdAfx.h"
#include "ImplConnections.h"

#include <CrySerialization/Enum.h>
#include <CrySerialization/Decorators/Range.h>

namespace ACE
{
namespace SDLMixer
{
static float const g_precision = 0.0001f;

//////////////////////////////////////////////////////////////////////////
void CEventConnection::Serialize(Serialization::IArchive& ar)
{
	EEventType const type = m_type;
	float const volume = m_volume;
	float const fadeInTime = m_fadeInTime;
	float const fadeOutTime = m_fadeOutTime;
	float const minAttenuation = m_minAttenuation;
	float const maxAttenuation = m_maxAttenuation;
	bool const isPanningEnabled = m_isPanningEnabled;
	bool const isAttenuationEnabled = m_isAttenuationEnabled;
	bool const isInfiniteLoop = m_isInfiniteLoop;
	uint32 const loopCount = m_loopCount;

	ar(m_type, "action", "Action");

	if (m_type == EEventType::Start)
	{
		ar(Serialization::Range(m_volume, -96.0f, 0.0f, 1.0f), "vol", "Volume (dB)");
		m_volume = crymath::clamp(m_volume, -96.0f, 0.0f);

		ar(Serialization::Range(m_fadeInTime, 0.0f, 60.0f, 0.5f), "fade_in_time", "Fade-In Time (sec)");
		m_fadeInTime = crymath::clamp(m_fadeInTime, 0.0f, 60.0f);

		ar(Serialization::Range(m_fadeOutTime, 0.0f, 60.0f, 0.5f), "fade_out_time", "Fade-Out Time (sec)");
		m_fadeOutTime = crymath::clamp(m_fadeOutTime, 0.0f, 60.0f);

		ar(m_isPanningEnabled, "panning", "Enable Panning");
		ar(m_isAttenuationEnabled, "attenuation", "Enable Attenuation");

		if (m_isAttenuationEnabled)
		{
			if (ar.isInput())
			{
				float minAtt = m_minAttenuation;
				float maxAtt = m_maxAttenuation;
				ar(minAtt, "min_att", "Min Distance");
				ar(maxAtt, "max_att", "Max Distance");

				minAtt = std::max(0.0f, minAtt);
				maxAtt = std::max(0.0f, maxAtt);

				if ((minAtt > maxAtt) || (fabs(minAtt - maxAtt) < g_precision))
				{
					minAtt = m_minAttenuation;
					maxAtt = m_maxAttenuation;
				}

				m_minAttenuation = minAtt;
				m_maxAttenuation = maxAtt;
			}
			else
			{
				ar(m_minAttenuation, "min_att", "Min Distance");
				ar(m_maxAttenuation, "max_att", "Max Distance");
			}
		}

		ar(m_isInfiniteLoop, "infinite_loop", "Infinite Loop");

		if (!m_isInfiniteLoop)
		{
			ar(m_loopCount, "loop_count", " Loop Count");
			m_loopCount = std::max<uint32>(1, m_loopCount);
		}
	}

	if (ar.isInput())
	{
		if (type != m_type ||
		    fabs(volume - m_volume) > g_precision ||
		    fabs(fadeInTime - m_fadeInTime) > g_precision ||
		    fabs(fadeOutTime - m_fadeOutTime) > g_precision ||
		    fabs(minAttenuation - m_minAttenuation) > g_precision ||
		    fabs(maxAttenuation - m_maxAttenuation) > g_precision ||
		    isPanningEnabled != m_isPanningEnabled ||
		    isAttenuationEnabled != m_isAttenuationEnabled ||
		    isInfiniteLoop != m_isInfiniteLoop ||
		    loopCount != m_loopCount)
		{
			SignalConnectionChanged();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CParameterConnection::Serialize(Serialization::IArchive& ar)
{
	float const mult = m_mult;
	float const shift = m_shift;

	ar(m_mult, "mult", "Multiply");
	m_mult = std::max(0.0f, m_mult);

	ar(m_shift, "shift", "Shift");

	if (ar.isInput())
	{
		if (fabs(mult - m_mult) > g_precision ||
		    fabs(shift - m_shift) > g_precision)
		{
			SignalConnectionChanged();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CStateConnection::Serialize(Serialization::IArchive& ar)
{
	float const value = m_value;

	ar(Serialization::Range(m_value, 0.0f, 1.0f, 0.1f), "value", "Volume (normalized)");
	m_value = crymath::clamp(m_value, 0.0f, 1.0f);

	if (ar.isInput())
	{
		if (fabs(value - m_value) > g_precision)
		{
			SignalConnectionChanged();
		}
	}
}

SERIALIZATION_ENUM_BEGIN(EEventType, "Event Type")
SERIALIZATION_ENUM(EEventType::Start, "start", "Start")
SERIALIZATION_ENUM(EEventType::Stop, "stop", "Stop")
SERIALIZATION_ENUM(EEventType::Pause, "pause", "Pause")
SERIALIZATION_ENUM(EEventType::Resume, "resume", "Resume")
SERIALIZATION_ENUM_END()
} // namespace SDLMixer
} // namespace ACE