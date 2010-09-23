#include "Alert.h"

Alert::Alert(const string& newMsg): Dialog(newMsg)
{
	// Make OK button.
	m_okayButton = new Button("okay");
}

Alert::~Alert()
{
	if (m_okayButton != NULL)
	{
		delete m_okayButton;
		m_okayButton = NULL;
	}
}

void Alert::drawObject(DirectX* pDirectX)
{
	// Draw like our parent.
	Dialog::drawObject(pDirectX);

	// Just add the button.
	m_okayButton->m_fX = m_fX + getWidth()/2 - m_okayButton->getWidth()/2;
	m_okayButton->m_fY = m_fY + getHeight() - m_okayButton->getHeight() - PADDING;
	m_okayButton->drawObject(pDirectX);
}

void Alert::mouseInteract()
{
	// Nothing yet.
}