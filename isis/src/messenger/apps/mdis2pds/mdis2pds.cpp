#include "Isis.h"

#include <cmath>
#include <string>
#include <sstream>
#include <fstream>

#include "UserInterface.h"
#include "Progress.h"
#include "FileName.h"
#include "IException.h"
#include "ProcessExportPds.h"
#include "Cube.h"
#include "Histogram.h"
#include "LineManager.h"
#include "OriginalLabel.h"
#include "PvlTranslationManager.h"
#include "BufferManager.h"
#include "LineManager.h"

using namespace std;
using namespace Isis;

inline QString Quote(const QString &value, const char qChar = '"') {
  if(value.isEmpty()) return (value);
  if(value[0] == qChar) return (value);
  return (QString(qChar + value + qChar));
}

inline double SetRound(double value, const int precision) {
  double scale = pow(10.0, precision);
  value = rint(value * scale) / scale;
  return (value);
}

inline void ValidateUnit(PvlKeyword &key, const QString &kunit) {
  PvlKeyword temp = key;
  key.clear();
  for(int i = 0 ; i < temp.size() ; i++) {
    try {
      //  If this works, check unit, otherwise an exception is thrown
      (void) toDouble(temp[i]);
      QString unit = temp.unit(i);
      if(unit.isEmpty()) unit = kunit;
      key.addValue(temp[i], unit);
    }
    catch(...) {
      key.addValue(temp[i]);
    }
  }
  return;
}

inline void FixUnit(PvlObject &obj, const QString &key, const QString &unit) {
  if(obj.hasKeyword(key, PvlObject::Traverse)) {
    ValidateUnit(obj.findKeyword(key, PvlObject::Traverse), unit);
  }
  return;
}

inline void FixQuotes(PvlContainer &kcont, const QString &value = "N/A") {
  PvlContainer::PvlKeywordIterator kiter;
  for(kiter = kcont.begin() ; kiter != kcont.end() ; ++kiter) {
    for(int nv = 0 ; nv < kiter->size() ; nv++) {
      if((*kiter)[nv] == value)(*kiter)[nv] = Quote((*kiter)[nv]);
    }
  }
}

inline void FixLabels(PvlObject &obj) {
  // Current object-owned keywords
  FixQuotes(obj);

  // Fix all nested objects
  PvlObject::PvlObjectIterator o;
  for(o = obj.beginObject() ; o != obj.endObject() ; ++o) {
    FixLabels(*o);
  }

  // Fix local groups
  PvlObject::PvlGroupIterator g;
  for(g = obj.beginGroup() ; g != obj.endGroup() ; ++g) {
    FixQuotes(*g);
  }
  return;
}


void IsisMain() {
  const QString mdis2pds_program = "mdis2pds";
  const QString mdis2pds_version = "1.0";
  const QString mdis2pds_revision = "$Revision: 5086 $";
  const QString mdis2pds_runtime = Application::DateTime();

  UserInterface &ui = Application::GetUserInterface();
  FileName input(ui.GetFileName("FROM"));
  FileName output = ui.GetFileName("TO");
  output = output.addExtension("IMG");

  // Set up the export
  ProcessExportPds processPds;
  Cube *incube = processPds.SetInputCube("FROM");

  Histogram *hist = incube->histogram(0);
  double minmin = 0.0;
  double maxmax = 0.0;
  if(ui.GetString("TYPE").compare("AUTOMATIC") == 0) {
    minmin = (ui.GetDouble("MINPER") <= 0.000001) ?
             hist->Minimum() :
             hist->Percent(ui.GetDouble("MINPER"));

    maxmax = (ui.GetDouble("MAXPER") >= 99.999999) ?
             hist->Maximum() :
             hist->Percent(ui.GetDouble("MAXPER"));
  }
  else {
    minmin = ui.GetDouble("MIN");
    maxmax = ui.GetDouble("MAX");
  }

  processPds.SetOutputEndian(Isis::Msb);
  processPds.SetExportType(ProcessExportPds::Fixed);
  processPds.SetInputRange(minmin, maxmax);

  // Set the output pixel type and the special pixel values
  QString dataSetID = "MESS-E/V/H-MDIS-";
  int nbits = ui.GetInteger("BITS");
  if(nbits == 8) {
    processPds.SetOutputType(UnsignedByte);
    processPds.SetOutputRange(VALID_MIN1, VALID_MAX1);
    processPds.SetOutputNull(NULL1);
    processPds.SetOutputLis(LOW_INSTR_SAT1);
    processPds.SetOutputLrs(LOW_REPR_SAT1);
    processPds.SetOutputHis(HIGH_INSTR_SAT1);
    processPds.SetOutputHrs(HIGH_REPR_SAT1);
    dataSetID += "1";
  }
  else if(nbits == 16) {
    processPds.SetOutputType(UnsignedWord);
    processPds.SetOutputRange(VALID_MINU2, VALID_MAXU2);
    processPds.SetOutputNull(NULLU2);
    processPds.SetOutputLis(LOW_INSTR_SATU2);
    processPds.SetOutputLrs(LOW_REPR_SATU2);
    processPds.SetOutputHis(HIGH_INSTR_SATU2);
    processPds.SetOutputHrs(HIGH_REPR_SATU2);
    dataSetID += "2";
  }
  else if(nbits == 32) {
    processPds.SetOutputType(Real);
    processPds.SetOutputRange(minmin, maxmax);
    processPds.SetOutputNull(NULL4);
    processPds.SetOutputLrs(LOW_REPR_SAT4);
    processPds.SetOutputLis(LOW_INSTR_SAT4);
    processPds.SetOutputHrs(HIGH_REPR_SAT4);
    processPds.SetOutputHis(HIGH_INSTR_SAT4);
    dataSetID += "4";
  }
  else if(nbits > 8  &&  nbits < 16) {
    processPds.SetOutputType(UnsignedWord);
    processPds.SetOutputRange(3.0, pow(2.0, (double)(nbits)) - 3.0);
    processPds.SetOutputNull(0);
    processPds.SetOutputLrs(1);
    processPds.SetOutputLis(2);
    processPds.SetOutputHis(pow(2.0, (double)(nbits)) - 2.0);
    processPds.SetOutputHrs(pow(2.0, (double)(nbits)) - 1.0);
    dataSetID += "0";
  }
  else {
    QString msg = "[" + QString(nbits) + "] is not a supported bit length.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  dataSetID += "-CDR-CALDATA-V1.0";

  Progress p;
  p.SetText("Modifying Keywords");
  p.SetMaximumSteps(7);
  p.CheckStatus();

  // Get the PDS label from the process
  Pvl &pdsLabel = processPds.StandardPdsLabel(ProcessExportPds::Image);

  // Translate the keywords from the original EDR PDS label that go in
  // this RDR PDS label
  OriginalLabel origBlob;
  incube->read(origBlob);
  Pvl origLabel;
  PvlObject origLabelObj = origBlob.ReturnLabels();
  origLabelObj.setName("OriginalLabelObject");
  origLabel.addObject(origLabelObj);

  p.CheckStatus();

  // Translates the ISIS labels along with the original EDR labels
  origLabel.addObject(*(incube->label()));
  PvlTranslationManager labels(origLabel,
                               "$messenger/translations/mdisCdrLabel.trn");
  labels.Auto(pdsLabel);

  p.CheckStatus();

  // Add keyword comments
  PvlKeyword &recordType(pdsLabel.findKeyword("RECORD_TYPE"));
  recordType.addComment("/*** FILE FORMAT ***/");

  PvlKeyword &image(pdsLabel.findKeyword("^IMAGE"));
  image.addComment("/*** POINTERS TO START BYTE OFFSET OF OBJECTS IN IMAGE FILE ***/");

  PvlKeyword &missionName(pdsLabel.findKeyword("MISSION_NAME"));
  missionName.addComment("/*** GENERAL DATA DESCRIPTION PARAMETERS ***/");

  PvlKeyword &startTime(pdsLabel.findKeyword("START_TIME"));
  startTime.addComment("/*** TIME PARAMETERS ***/");

  PvlKeyword &instrumentName(pdsLabel.findKeyword("INSTRUMENT_NAME"));
  instrumentName.addComment("/*** INSTRUMENT ENGINEERING PARAMETERS ***/");

  PvlKeyword &messMetExp(pdsLabel.findKeyword("MESS:MET_EXP"));
  messMetExp.addComment("/*** INSTRUMENT RAW PARAMETERS ***/");

  PvlKeyword &geometry(pdsLabel.findKeyword("RIGHT_ASCENSION"));
  geometry.addComment("/*** GEOMETRY INFORMATION ***/");

  PvlKeyword &target(pdsLabel.findKeyword("SC_TARGET_POSITION_VECTOR"));
  target.addComment("/*** TARGET PARAMETERS ***/");

  PvlKeyword &sensor(pdsLabel.findKeyword("SLANT_DISTANCE"));
  sensor.addComment("/*** TARGET WITHIN SENSOR FOV ***/");

  PvlKeyword &spacecraftPosition(pdsLabel.findKeyword("SUB_SPACECRAFT_LATITUDE"));
  spacecraftPosition.addComment("/*** SPACECRAFT POSITION WITH RESPECT TO CENTRAL BODY ***/");

  PvlKeyword &spacecraftLocation(pdsLabel.findKeyword("SPACECRAFT_SOLAR_DISTANCE"));
  spacecraftLocation.addComment("/*** SPACECRAFT LOCATION ***/");

  PvlKeyword &solarDistance(pdsLabel.findKeyword("SOLAR_DISTANCE"));
  solarDistance.addComment("/*** VIEWING AND LIGHTING GEOMETRY (SUN ON TARGET) ***/");

  PvlGroup &subframe(pdsLabel.findGroup("SUBFRAME1_PARAMETERS"));
  subframe.addComment("/*** GEOMETRY FOR EACH SUBFRAME ***/");

  p.CheckStatus();

  // Creates keywords from the input's hist above
  PvlKeyword minDn("MINIMUM", toString(SetRound(hist->Minimum(), 16)));
  PvlKeyword maxDn("MAXIMUM", toString(SetRound(hist->Maximum(), 16)));
  PvlKeyword meanDn("MEAN", toString(SetRound(hist->Average(), 16)));
  PvlKeyword stddev("STANDARD_DEVIATION", toString(SetRound(hist->StandardDeviation(), 16)));

  PvlKeyword saturated("SATURATED_PIXEL_COUNT", toString(hist->HisPixels()));

  PvlObject &imageObj = pdsLabel.findObject("IMAGE");

  minDn.addComment("/*** IMAGE STATISTICS ***/");
  imageObj.addKeyword(minDn);
  imageObj.addKeyword(maxDn);
  imageObj.addKeyword(meanDn);
  imageObj.addKeyword(stddev);
  saturated.addComment("/*** PIXEL COUNTS ***/");
  imageObj.addKeyword(saturated);
  if(imageObj.hasKeyword("DARK_STRIP_MEAN")) {
    PvlKeyword &darkStripMean = imageObj.findKeyword("DARK_STRIP_MEAN");

    try {
      if(darkStripMean.size() > 0) {
        darkStripMean[0] = toString(SetRound(toDouble(darkStripMean[0]), 16));
      }
    }
    catch(IException &) {
      // If we fail to convert this keyword to a number, then preserve
      // its existing value
    }
  }

  p.CheckStatus();

  // Fixes bad keywords
  PvlKeyword &data_set_id = pdsLabel.findKeyword("DATA_SET_ID", Pvl::Traverse);
  data_set_id.setValue(dataSetID);
  PvlKeyword &product_id = pdsLabel.findKeyword("PRODUCT_ID", Pvl::Traverse);
  if((product_id.size() == 0) || ((product_id.size() > 0) && (product_id[0] == "N/A"))) {
    product_id.setValue(output.baseName());
  }
  PvlKeyword &product_creation_time = pdsLabel.findKeyword("PRODUCT_CREATION_TIME", Pvl::Traverse);
  product_creation_time.setValue(mdis2pds_runtime);

  PvlKeyword &software_name = pdsLabel.findKeyword("SOFTWARE_NAME", Pvl::Traverse);
  if((software_name.size() > 0) && (software_name[0] == "N/A")) {
    software_name.setValue(mdis2pds_program);
  }

  PvlKeyword &software_version_id = pdsLabel.findKeyword("SOFTWARE_VERSION_ID", Pvl::Traverse);
  if(software_version_id.size() > 0) {
    if(software_version_id[0] == "N/A") {
      software_version_id.setValue(Quote(mdis2pds_version));
    }
    else {
      software_version_id.setValue(software_version_id[0]);
    }
  }

  PvlKeyword &filter_number = pdsLabel.findKeyword("FILTER_NUMBER", Pvl::Traverse);
  if((filter_number.size() > 0)) {
    filter_number.setValue(Quote(filter_number[0]));
  }


  // Add quotes
  PvlKeyword &data_quality_id = pdsLabel.findKeyword("DATA_QUALITY_ID", Pvl::Traverse);
  data_quality_id.setValue(Quote(data_quality_id));
  PvlKeyword &sequence_name = pdsLabel.findKeyword("SEQUENCE_NAME", Pvl::Traverse);
  sequence_name.setValue(Quote(sequence_name));

  PvlKeyword &start_count = pdsLabel.findKeyword("SPACECRAFT_CLOCK_START_COUNT", Pvl::Traverse);
  start_count.setValue(Quote(start_count));
  PvlKeyword &stop_count = pdsLabel.findKeyword("SPACECRAFT_CLOCK_STOP_COUNT", Pvl::Traverse);
  stop_count.setValue(Quote(stop_count));

  PvlKeyword &site_id = pdsLabel.findKeyword("SITE_ID", Pvl::Traverse);
  site_id.setValue(Quote(site_id));
  PvlKeyword &source_product_id = pdsLabel.findKeyword("SOURCE_PRODUCT_ID", Pvl::Traverse);
  for(int i = 0; i < source_product_id.size(); i++) {
    source_product_id[i] = Quote(source_product_id[i]);
  }

  //  Enforce parentheses for scalars
  if(source_product_id.size() == 1)
    source_product_id.setValue('(' + source_product_id[0] + ')');

  // Removes keywords
  PvlObject &imageObject(pdsLabel.findObject("IMAGE"));
  imageObject.deleteKeyword("FILTER_NAME");
  imageObject.deleteKeyword("CENTER_FILTER_WAVELENGTH");
  imageObject.deleteKeyword("BANDWIDTH");

  p.CheckStatus();

  //  Fix all the hosed units upon ingest.  They are illformed.
  FixUnit(pdsLabel, "RETICLE_POINT_RA", "DEG");
  FixUnit(pdsLabel, "RETICLE_POINT_DECLINATION", "DEG");
  FixUnit(pdsLabel, "RETICLE_POINT_LATITUDE", "DEG");
  FixUnit(pdsLabel, "RETICLE_POINT_LONGITUDE", "DEG");

  //  Now address nested keywords in SUBFRAME groups
  for(int i = 1 ; i <= 5 ; i++) {
    QString n(toString(i));
    QString group = "SUBFRAME" + n + "_PARAMETERS";
    if(pdsLabel.hasGroup(group)) {
      PvlGroup &grp = pdsLabel.findGroup(group);
      ValidateUnit(grp.findKeyword("RETICLE_POINT_LATITUDE"), "DEG");
      ValidateUnit(grp.findKeyword("RETICLE_POINT_LONGITUDE"), "DEG");
    }
  }

  p.CheckStatus();

//  Finally, fix keywords by Quoting missing N/A values
  FixLabels(pdsLabel);
  p.CheckStatus();

  // All done...write result.
  ofstream outstream(output.expanded().toAscii().data());
  processPds.OutputLabel(outstream);

  processPds.StartProcess(outstream);
  outstream.close();
  processPds.EndProcess();
}