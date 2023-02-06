#ifndef VOLUME_DEMO_MODELING_H__
#define VOLUME_DEMO_MODELING_H__

#include "polygonobject.h"

//---------------------------------------------------------------------------
/// Creates a triangle.
/// @param[in]	poly	PolygonObject to fill.
/// @return				False if an error occurred.
//---------------------------------------------------------------------------
bool CreateTriangle(PolygonObject& poly);

//---------------------------------------------------------------------------
/// Creates a plane (two triangles).
/// @param[in]	poly	PolygonObject to fill.
/// @return				False if an error occurred.
//---------------------------------------------------------------------------
bool CreatePlane(PolygonObject& poly);

#endif // VOLUME_DEMO_MODELING_H__
