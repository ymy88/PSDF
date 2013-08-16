#pragma once

#include "../Common/Common.h"

class LayerManager;

//------------------------------------
// Layer
//------------------------------------
class PSDF_CORE_DLL_DECL Layer : public Group
{
	friend class LayerManager;

private:
	Layer() {}
	virtual ~Layer(){};

public:
	void			show() { setNodeMask( _backupNodeMask ); }
	void			hide() { _backupNodeMask = getNodeMask(); setNodeMask( 0x00000000 ); }
	bool			isShow() { return getNodeMask() != 0; }

	void			set_z_index( unsigned int z ) { getOrCreateStateSet()->setRenderBinDetails(z, "RenderBin"); _z_index = z; }
	unsigned int	get_z_index(){ return _z_index; }

private:
	unsigned int	_z_index;
	unsigned int	_backupNodeMask;
};

