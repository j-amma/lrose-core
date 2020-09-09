// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 

#include "RhiPlot.hh"
#include "PolarManager.hh"
#include "SpreadSheetView.hh"
#include "SpreadSheetController.hh"
#include "ParameterColorView.hh"
#include "FieldColorController.hh"
#include "DisplayFieldModel.hh"
#include "BoundaryPointEditor.hh"
#include "SpreadSheetView.hh"

#include <toolsa/toolsa_macros.h>
#include <toolsa/LogStream.hh>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QErrorMessage>
#include <QRect>

using namespace std;

RhiPlot::RhiPlot(PolarWidget* parent,
                 const PolarManager &manager,
                 const Params &params,
                 int id,
                 Params::plot_type_t plotType,
                 string label,
                 double minAz,
                 double maxAz,
                 double minEl,
                 double maxEl,
                 double minXKm,
                 double maxXKm,
                 double minYKm,
                 double maxYKm,
                 const RadxPlatform &platform,
                 const vector<DisplayField *> &fields,
                 bool haveFilteredFields) :
        PolarPlot(parent, manager, params, id, plotType, label,
                  minAz, maxAz, minEl, maxEl,
                  minXKm, maxXKm, minYKm, maxYKm,
                  platform, fields, haveFilteredFields)
        
{

   // initialize world view

  _maxRangeKm = _params.max_range_km;
  _initWorld();

  // set up overlays
  
  setGrids(_params.ppi_grids_on_at_startup);
  setRings(_params.ppi_range_rings_on_at_startup);
  setAngleLines(_params.ppi_azimuth_lines_on_at_startup);

  // archive mode
  
  _isArchiveMode = false;

  _plotStartTime.set(0);
  _plotEndTime.set(0);
  _meanElev = -9999.0;
  _sumElev = 0.0;
  _nRays = 0.0;

  // set up ray locator array
  
  _rayLocWidthHalf =
    (int) (_params.rhi_rendering_beam_width * RAY_LOC_RES + 0.5);
  
  for (int ii = 0; ii < RAY_LOC_N; ii++) {
    RayLoc *loc = new RayLoc(ii);
    _rayLoc.push_back(loc);
  }

}

/*************************************************************************
 * Destructor
 */

RhiPlot::~RhiPlot()
{

  // clear ray locator
  
  for (size_t ii = 0; ii < _rayLoc.size(); ii++) {
    delete _rayLoc[ii];
  }
  _rayLoc.clear();
  
}

/*************************************************************************
 * clear()
 */

void RhiPlot::clear()
{

  for (int ii = 0; ii < RAY_LOC_N; ii++) {
    _rayLoc[ii]->clearData();
  }
  
  // Clear out the beam array
  
  // for (size_t i = 0; i < _ppiBeams.size(); i++) {
  //   Beam::deleteIfUnused(_ppiBeams[i]);
  // }
  // _ppiBeams.clear();
  
  // Now rerender the images
  
  refreshFieldImages();
  _parent->showOpeningFileMsg(false);
}

/*************************************************************************
 * selectVar()
 */

void RhiPlot::selectVar(const size_t fieldNum)
{
  
  // If the field index isn't actually changing, we don't need to do anything
  
  if (_fieldNum == fieldNum) {
    return;
  }
  
  if (_params.debug >= Params::DEBUG_VERBOSE) {
    cerr << "=========>> RhiPlot::selectVar() for fieldNum: " 
         << fieldNum << endl;
  }

  // If this field isn't being rendered in the background, render all of
  // the beams for it

  if (!_fieldRenderers[fieldNum]->isBackgroundRendered()) {
    for (size_t ii = 0; ii < _rayLoc.size(); ii++) {
      if (_rayLoc[ii]->getActive()) {
        RhiBeam *beam = _rayLoc[ii]->getBeam();
        beam->setBeingRendered(fieldNum, true);
        _fieldRenderers[fieldNum]->addBeam(beam);
      }
    } // ii
  }
  _performRendering();

  // Do any needed housekeeping when the field selection is changed

  _fieldRenderers[_fieldNum]->unselectField();
  _fieldRenderers[fieldNum]->selectField();
  
  // Change the selected field fieldNum

  _fieldNum = fieldNum;

  // Update the display

  _parent->update();
}


/*************************************************************************
 * clearVar()
 */

void RhiPlot::clearVar(const size_t index)
{

  if (index >= _fields.size()) {
    return;
  }

  // Set the brush for every beam/gate for this field to use the background
  // color

  for (size_t ii = 0; ii < _rayLoc.size(); ii++) {
    if (_rayLoc[ii]->getActive()) {
      RhiBeam *beam = _rayLoc[ii]->getBeam();
      beam->resetFieldBrush(index, &_backgroundBrush);
    }
  } // ii
  
  if (index == _fieldNum) {
    _parent->update();
  }

}


/*************************************************************************
 * addRay()
 */

void RhiPlot::addRay(const RadxRay *ray,
                     const std::vector< std::vector< double > > &beam_data,
                     const std::vector< DisplayField* > &fields)

{

  LOG(DEBUG) << "enter";
  
  if (ray->getSweepMode() != Radx::SWEEP_MODE_RHI) {
    return;
  }

  double az = ray->getAzimuthDeg();
  double el = ray->getElevationDeg();

  // check that we should process this beam

  if (el < _minEl || el > _maxEl) {
    return;
  }
  if (_minAz < 0) {
    double azTest = az;
    if (azTest > _maxAz) {
      azTest -= 360.0;
    }
    if (azTest < _minAz || azTest > _maxAz) {
      return;
    }
  } else {
    if (az < _minAz || az > _maxAz) {
      return;
    }
  }

  // Determine the extent of this ray

  double halfAngle = _params.ppi_rendering_beam_width / 2.0;
  double startEl = el - halfAngle;
  double endEl = el + halfAngle;
  
  // create beam

  double instHtKm = 0.0;
  RhiBeam *beam = new RhiBeam(_params, ray, instHtKm,
                              _fields.size(), startEl, endEl);

  // store the ray and beam data
  
  _storeRayLoc(el, ray, beam);

  // Set up the brushes for all of the fields in this beam.  This can be
  // done independently of a Painter object.
  
  beam->fillColors(beam_data, fields, &_backgroundBrush);
  
  // Add the new beams to the render lists for each of the fields
  
  for (size_t field = 0; field < _fieldRenderers.size(); ++field) {
    if (field == _fieldNum ||
        _fieldRenderers[field]->isBackgroundRendered()) {
      _fieldRenderers[field]->addBeam(beam);
    } else {
      beam->setBeingRendered(field, false);
    }
  }
    
  // Start the threads to render the new beams

  _performRendering();
  
}


/**************************************`***********************************
 * initialize world coords
 */

void RhiPlot::_initWorld()
{

  // Set the ring spacing.  This is dependent on the value of _maxRange.

  _setGridSpacing();
  
  // set world view

  int leftMargin = 0;
  int rightMargin = 0;
  int topMargin = 0;
  int bottomMargin = 0;
  int colorScaleWidth = 0;
  int axisTickLen = 7;
  int nTicksIdeal = 7;
  int textMargin = 5;
  
  _fullWorld.setLeftMargin(leftMargin);
  _fullWorld.setRightMargin(rightMargin);
  _fullWorld.setTopMargin(topMargin);
  _fullWorld.setBottomMargin(bottomMargin);
  _fullWorld.setColorScaleWidth(colorScaleWidth);
  _fullWorld.setWorldLimits(_minXKm, _minYKm, _maxXKm, _maxYKm);
  _fullWorld.setXAxisTickLen(axisTickLen);
  _fullWorld.setXNTicksIdeal(nTicksIdeal);
  _fullWorld.setAxisTextMargin(textMargin);
  
  _zoomWorld = _fullWorld;
  _isZoomed = false;
  setTransform00(_zoomWorld.getTransform());
  _setGridSpacing();

  // Initialize the images used for double-buffering.  For some reason,
  // the window size is incorrect at this point, but that will be corrected
  // by the system with a call to resize().

  refreshFieldImages();
  
}

////////////////////////////////////////////////////////////////////////////
// get ray closest to click point

const RadxRay *RhiPlot::getClosestRay(int imageX, int imageY,
                                      double &xKm, double &yKm)
  
{

  // compute location in world coords
  
  xKm = _zoomWorld.getXWorld(imageX);
  yKm = _zoomWorld.getYWorld(imageY);

  // compute the azimuth relative to the display
  
  double clickEl = atan2(yKm, xKm) * RAD_TO_DEG;
  double radarDisplayEl = 90.0 - clickEl;
  if (radarDisplayEl < 0.0) radarDisplayEl += 360.0;
  LOG(DEBUG) << "clickEl = " << clickEl << " from xKm, yKm = " 
             << xKm << "," << yKm; 
  LOG(DEBUG) << "radarDisplayEl = " << radarDisplayEl << " from xKm, yKm = "
             << xKm << yKm;

  // search for the closest ray to this el
  
  int rayLocIndex = _getRayLocIndex(radarDisplayEl);
  for (int ii = 0; ii <= _rayLocWidthHalf * 2; ii++) {
    int jj1 = (rayLocIndex - ii + RAY_LOC_N) % RAY_LOC_N;
    if (_rayLoc[jj1]->getActive()) {
      return _rayLoc[jj1]->getRay();
    }
    int jj2 = (rayLocIndex + ii + RAY_LOC_N) % RAY_LOC_N;
    if (_rayLoc[jj2]->getActive()) {
      return _rayLoc[jj2]->getRay();
    }
  }

  // no active ray close by

  return NULL;

}

/*************************************************************************
 * _setGridSpacing()
 */

void RhiPlot::_setGridSpacing()
{

  double xRange = _zoomWorld.getXMaxWorld() - _zoomWorld.getXMinWorld();
  double yRange = _zoomWorld.getYMaxWorld() - _zoomWorld.getYMinWorld();
  double diagonal = sqrt(xRange * xRange + yRange * yRange);

  if (diagonal <= 1.0) {
    _ringSpacing = 0.05;
  } else if (diagonal <= 2.0) {
    _ringSpacing = 0.1;
  } else if (diagonal <= 5.0) {
    _ringSpacing = 0.2;
  } else if (diagonal <= 10.0) {
    _ringSpacing = 0.5;
  } else if (diagonal <= 20.0) {
    _ringSpacing = 1.0;
  } else if (diagonal <= 50.0) {
    _ringSpacing = 2.0;
  } else if (diagonal <= 100.0) {
    _ringSpacing = 5.0;
  } else if (diagonal <= 200.0) {
    _ringSpacing = 10.0;
  } else if (diagonal <= 300.0) {
    _ringSpacing = 20.0;
  } else if (diagonal <= 400.0) {
    _ringSpacing = 25.0;
  } else if (diagonal <= 500.0) {
    _ringSpacing = 50.0;
  } else {
    _ringSpacing = 50.0;
  }

}


/*************************************************************************
 * _drawOverlays()
 */

void RhiPlot::_drawOverlays(QPainter &painter)
{

  // Don't try to draw rings if we haven't been configured yet or if the
  // rings or grids aren't enabled.
  
  if (!_ringsEnabled && !_gridsEnabled && !_angleLinesEnabled) {
    return;
  }

  // save painter state, set transform

  painter.save();

  // store font
  
  QFont origFont = painter.font();
  
  // Draw rings

  if (_ringSpacing > 0.0 && _ringsEnabled) {
    
    // Set up the painter for rings
    
    painter.setPen(_gridRingsColor);
    
    // set narrow line width
    QPen pen = painter.pen();
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setPen(_gridRingsColor);
    
    painter.save();
    painter.setTransform(_zoomWorld.getTransform());
    pen.setWidth(1);
    double ringRange = _ringSpacing;
    while (ringRange <= _maxRangeKm) {
      // _zoomWorld.drawArc(painter, 0.0, 0.0, ringRange, ringRange, 0.0, 360.0);
      QRectF rect(-ringRange, -ringRange, ringRange * 2.0, ringRange * 2.0);
      painter.drawEllipse(rect);
      ringRange += _ringSpacing;
    }
    painter.restore();
  
    // Draw the labels
    
    QFont font = painter.font();
    font.setPointSizeF(_params.range_ring_label_font_size);
    painter.setFont(font);
    painter.setPen(_gridRingsColor);
    // painter.setWindow(0, 0, width(), height());
    
    ringRange = _ringSpacing;
    while (ringRange <= _maxRangeKm) {
      double labelPos = ringRange * SIN_45;
      const string &labelStr = _scaledLabel.scale(ringRange);
      _zoomWorld.drawText(painter, labelStr, labelPos, labelPos, Qt::AlignCenter);
      _zoomWorld.drawText(painter, labelStr, -labelPos, labelPos, Qt::AlignCenter);
      _zoomWorld.drawText(painter, labelStr, labelPos, -labelPos, Qt::AlignCenter);
      _zoomWorld.drawText(painter, labelStr, -labelPos, -labelPos, Qt::AlignCenter);
      ringRange += _ringSpacing;
    }

  } /* endif - draw rings */
  
  // Draw the grid

  if (_ringSpacing > 0.0 && _gridsEnabled)  {
    
    // Set up the painter
    
    painter.setPen(_gridRingsColor);
  
    double ringRange = _ringSpacing;
    double maxRingRange = ringRange;
    while (ringRange <= _maxRangeKm) {
      
      _zoomWorld.drawLine(painter, ringRange, -_maxRangeKm, ringRange, _maxRangeKm);
      _zoomWorld.drawLine(painter, -ringRange, -_maxRangeKm, -ringRange, _maxRangeKm);
      _zoomWorld.drawLine(painter, -_maxRangeKm, ringRange, _maxRangeKm, ringRange);
      _zoomWorld.drawLine(painter, -_maxRangeKm, -ringRange, _maxRangeKm, -ringRange);
      
      maxRingRange = ringRange;
      ringRange += _ringSpacing;
    }

    _zoomWorld.specifyXTicks(-maxRingRange, _ringSpacing);
    _zoomWorld.specifyYTicks(-maxRingRange, _ringSpacing);

    _zoomWorld.drawAxisLeft(painter, "km", true, true, true, false);
    _zoomWorld.drawAxisRight(painter, "km", true, true, true, false);
    _zoomWorld.drawAxisTop(painter, "km", true, true, true, false);
    _zoomWorld.drawAxisBottom(painter, "km", true, true, true, false);
    
    _zoomWorld.unspecifyXTicks();
    _zoomWorld.unspecifyYTicks();
    
  }
  
  // Draw the azimuth lines

  if (_angleLinesEnabled) {

    // Set up the painter
    
    painter.setPen(_gridRingsColor);
    
    // Draw the lines along the X and Y axes

    _zoomWorld.drawLine(painter, 0, -_maxRangeKm, 0, _maxRangeKm);
    _zoomWorld.drawLine(painter, -_maxRangeKm, 0, _maxRangeKm, 0);
    
    // Draw the lines along the 30 degree lines

    double end_pos1 = SIN_30 * _maxRangeKm;
    double end_pos2 = COS_30 * _maxRangeKm;
    
    _zoomWorld.drawLine(painter, end_pos1, end_pos2, -end_pos1, -end_pos2);
    _zoomWorld.drawLine(painter, end_pos2, end_pos1, -end_pos2, -end_pos1);
    _zoomWorld.drawLine(painter, -end_pos1, end_pos2, end_pos1, -end_pos2);
    _zoomWorld.drawLine(painter, end_pos2, -end_pos1, -end_pos2, end_pos1);

  }
  
  // add label
  
  if (_label.size() > 0) {
    
    QFont ufont(painter.font());
    ufont.setPointSizeF(_params.main_label_font_size);
    painter.setFont(ufont);
    
    QRect tRect(painter.fontMetrics().tightBoundingRect(_label.c_str()));
    int iyy = 5;
    int ixx = 5;
    QString qlabel(_label.c_str());
    painter.drawText(ixx, iyy, tRect.width() + 4, tRect.height() + 4, 
                     Qt::AlignTop | Qt::AlignHCenter, qlabel);
    
  }

  // reset painter state
  
  painter.restore();

  if (_archiveMode) {

    // add legends with time, field name and elevation angle

    vector<string> legends;
    char text[1024];

    // time legend

    sprintf(text, "Start time: %s", _plotStartTime.asString(0).c_str());
    legends.push_back(text);
    
    // radar and site name legend

    string radarName(_platform.getInstrumentName());
    if (_params.override_radar_name) {
      radarName = _params.radar_name;
    }
    string siteName(_platform.getInstrumentName());
    if (_params.override_site_name) {
      siteName = _params.site_name;
    }
    string radarSiteLabel = radarName;
    if (siteName.size() > 0 && siteName != radarName) {
      radarSiteLabel += "/";
      radarSiteLabel += siteName;
    }
    legends.push_back(radarSiteLabel);

    // field name legend

    string fieldName = _fieldRenderers[_fieldNum]->getField().getLabel();
    sprintf(text, "Field: %s", fieldName.c_str());
    legends.push_back(text);

    // elevation legend

    sprintf(text, "Elevation(deg): %.2f", _meanElev);
    legends.push_back(text);

    // nrays legend

    sprintf(text, "NRays: %g", _nRays);
    legends.push_back(text);
    
    painter.save();
    painter.setPen(Qt::yellow);
    painter.setBrush(Qt::black);
    painter.setBackgroundMode(Qt::OpaqueMode);

    switch (_params.ppi_main_legend_pos) {
      case Params::LEGEND_TOP_LEFT:
        _zoomWorld.drawLegendsTopLeft(painter, legends);
        break;
      case Params::LEGEND_TOP_RIGHT:
        _zoomWorld.drawLegendsTopRight(painter, legends);
        break;
      case Params::LEGEND_BOTTOM_LEFT:
        _zoomWorld.drawLegendsBottomLeft(painter, legends);
        break;
      case Params::LEGEND_BOTTOM_RIGHT:
        _zoomWorld.drawLegendsBottomRight(painter, legends);
        break;
      default: {}
    }
    
    painter.restore();

  } // if (_archiveMode) {

}

///////////////////////////////////////////////////////////////////////////
// Draw text, with (X, Y) in screen space
//
// Flags give the justification in Qt, and are or'd from the following:
//    Qt::AlignLeft aligns to the left border.
//    Qt::AlignRight aligns to the right border.
//    Qt::AlignJustify produces justified text.
//    Qt::AlignHCenter aligns horizontally centered.
//    Qt::AlignTop aligns to the top border.
//    Qt::AlignBottom aligns to the bottom border.
//    Qt::AlignVCenter aligns vertically centered
//    Qt::AlignCenter (== Qt::AlignHCenter | Qt::AlignVCenter)
//    Qt::TextSingleLine ignores newline characters in the text.
//    Qt::TextExpandTabs expands tabs (see below)
//    Qt::TextShowMnemonic interprets "&x" as x; i.e., underlined.
//    Qt::TextWordWrap breaks the text to fit the rectangle.

// draw text in world coords

void RhiPlot::_drawScreenText(QPainter &painter, const string &text,
                              int text_x, int text_y,
                              int flags)
  
{

  int ixx = text_x;
  int iyy = text_y;
	
  QRect tRect(painter.fontMetrics().tightBoundingRect(text.c_str()));
  QRect bRect(painter.fontMetrics().
              boundingRect(ixx, iyy,
                           tRect.width() + 2, tRect.height() + 2,
                           flags, text.c_str()));
    
  painter.drawText(bRect, flags, text.c_str());
    
}

/*************************************************************************
 * numBeams()
 */

size_t RhiPlot::getNumBeams() const
{
  size_t count = 0;
  for (size_t ii = 0; ii < _rayLoc.size(); ii++) {
    if (_rayLoc[ii]->getActive()) {
      count++;
    }
  }
  return count;
}

/*************************************************************************
 * refreshFieldImages()
 */

void RhiPlot::refreshFieldImages()
{

  for (size_t ifield = 0; ifield < _fieldRenderers.size(); ++ifield) {
    
    FieldRenderer *field = _fieldRenderers[ifield];
    
    // If needed, create new image for this field

    QSize imageSize = field->getImage()->size();
    if (imageSize.width() != _fullWorld.getWidthPixels() ||
        imageSize.height() != _fullWorld.getHeightPixels()) {
      field->createImage(_fullWorld.getWidthPixels(),
                         _fullWorld.getHeightPixels());
    }

    // clear image

    field->getImage()->fill(_backgroundBrush.color().rgb());
    
    // set up rendering details
    
    field->setTransform(_zoomWorld.getTransform());
    
    // Add pointers to the beams to be rendered
    
    if (ifield == _fieldNum || field->isBackgroundRendered()) {
      for (size_t ii = 0; ii < _rayLoc.size(); ii++) {
        if (_rayLoc[ii]->getActive()) {
          RhiBeam *beam = _rayLoc[ii]->getBeam();
          beam->setBeingRendered(ifield, true);
          field->addBeam(beam);
        }
      } // ii
    }
    
  } // ifield
  
  // do the rendering

  _performRendering();

  // update main plot
  
  _parent->update();

}

/*************************************************************************
 * RayLoc inner class
 */

// constructor

RhiPlot::RayLoc::RayLoc(int index)
{
  _index = index;
  _midEl = (double) (index - RAY_LOC_NHALF) / (double) RAY_LOC_RES;
  _trueEl = _midEl;
  _active = false;
  _ray = NULL;
  _beam = NULL;
}

// set the data

void RhiPlot::RayLoc::setData(double el,
                              const RadxRay *ray,
                              RhiBeam *beam)
{
  clearData();
  _trueEl = el;
  _ray = ray;
  _ray->addClient();
  _beam = beam;
  _beam->addClient();
  _active = true;
}

// clear ray and beam data

void RhiPlot::RayLoc::clearData()
{
  if (_ray) {
    RadxRay::deleteIfUnused(_ray);
    _ray = NULL;
  }
  if (_beam) {
    Beam::deleteIfUnused(_beam);
    _beam = NULL;
  }
  _active = false;
}

// compute the ray loc index from the elevation

int RhiPlot::_getRayLocIndex(double el)
{
  int iel = (int) floor(el * RAY_LOC_RES + 0.5) + RAY_LOC_NHALF;
  iel = (iel + RAY_LOC_N) % RAY_LOC_N;
  return iel;
}

// store the ray at the appropriate location

void RhiPlot::_storeRayLoc(double el,
                           const RadxRay *ray,
                           RhiBeam *beam)
{

  int index = _getRayLocIndex(el);

  // clear any existing data in ray width

  for (int ii = -_rayLocWidthHalf; ii <= _rayLocWidthHalf; ii++) {
    int jj = (ii + RAY_LOC_N) % RAY_LOC_N;
    _rayLoc[jj]->clearData();
  }

  // store this ray
  
  _rayLoc[index]->setData(el, ray, beam);

}

