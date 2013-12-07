#ifndef MocNarrowAngleCamera_h
#define MocNarrowAngleCamera_h
/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "LineScanCamera.h"

namespace Isis {
  /**
   * @brief MOC Narrow Angle Camera Model
   *
   * This is the camera model for the Mars Global Surveyor MOC narrow angle 
   * camera. 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsGlobalSurveyor
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *            method instead of CreateCache(...).
   *   @history 2009-03-07 Debbie A. Cook Removed reference to obsolute
   *            CameraDetectorMap methods
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test
   *            for new methods. Updated documentation. Removed Mgs namespace
   *            wrap inside Isis namespace. Added Isis Disclaimer to files.
   *            Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *            coding standards. References #972.
   */
  class MocNarrowAngleCamera : public LineScanCamera {
    public:
      MocNarrowAngleCamera(Pvl &lab);

      //! Destroys the MocNarrowAngleCamera object
      ~MocNarrowAngleCamera() {};

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-94000); }

      /** 
       * CK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /** 
       * SPK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif