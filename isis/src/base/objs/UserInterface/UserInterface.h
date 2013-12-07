#ifndef UserInterface_h
#define UserInterface_h
/**
 * @file
 * $Revision: 1.14 $
 * $Date: 2010/05/28 17:55:36 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "IsisAml.h"
#include "PvlTokenizer.h"
#include "FileName.h"
#include "Gui.h"

class Gui;

namespace Isis {
  /**
   * @brief  Command Line and Xml loader, validation, and access
   *
   * This object is used to load and query user input via the command line. It
   * requires as input to the constructor 1) an Isis Application Xml file and 2)
   * the command line arguments (argc and argv). The Xml file will be used to
   * validate the user input given on the command line (if any). To access user
   * input see the Aml class which is inherited.
   *
   * @ingroup ApplicationInterface
   *
   * @author 2002-05-29 Jeff Anderson
   *
   * @internal
   *   @history 2002-10-25 Jeff Anderson - Command line mode was not fully
   *                           verifying the AML object. Invoked the VerifyAll
   *                           method after loading each of the command line
   *                           tokens.
   *   @history 2003-02-07 Jeff Anderson - Modified constructor so that it will
   *                           not start the GUI if the program name is
   *                           unitTest.
   *   @history 2003-02-12 Jeff Anderson - Strip off leading directory in front
   *                           of argv[0] so that unit tests run with
   *                           pathnames do not start the Isis Gui.
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2003-12-16 Jeff Anderson - Added command line option -LAST and
   *                           RESTORE=file.par
   *   @history 2004-02-26 Jeff Anderson - Added command line option -HELP
   *   @history 2004-02-26 Jeff Anderson - Modified to allow a parameter to
   *                           appear multiple times on the command line
   *   @history 2004-02-29 Jeff Anderson - Added the -PID command line switch
   *                           which allows interprocess communication to
   *                           occur with the parent so that the parents GUI can
   *                           be properly updated.
   *   @history 2005-02-22 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2005-10-03 Elizabeth Miller - changed @ingroup tag
   *   @history 2005-12-21 Elizabeth Miller - Added command line options
   *                           -BATCHLIST, -SAVE, -ERRLIST, -ONERROR, 
   *                           -PREFERENCE, and -PRINTFILE
   *   @history 2006-01-23 Elizabeth Miller - Renamed -HELP to -WEBHELP and made
   *                           it accept abbreviations of reserve params
   *   @history 2007-07-12 Steven Koechle - Added -NOGUI flag
   *   @history 2007-10-04 Steven Koechle - Added -info flag. Debugging option
   *                           to create a log of system info.
   *   @history 2008-02-22 Steven Koechle - Modified batchlist to take tab,
   *                           command, and space characters as delimiters but
   *                           also allow special cases like tab, as a single
   *                           delimiter leaves quoted strings alone.
   *   @history 2008-04-16 Steven Lambright - Moved parameter verification call
   *   @history 2008-06-06 Steven Lambright - Changed corrupt history file
   *                           message
   *   @history 2008-06-18 Steven Lambright - Fixed documentation
   *   @history 2008-09-23 Christopher Austin - Added a try/catch to
   *                           SaveHistory(), where if the history file is
   *                           corrupt, it simply overwrites it with the new
   *                           single valid entry.
   *   @history 2008-01-07 Steven Lambright - Changed unit test and error on
   *                           invalid parameter history files to conform with a
   *                           FileName class change where expanded(...) always
   *                           returns a full path.
   *   @history 2009-08-17 Steven Lambright - Parameters are now more correctly
   *                           interpretted from argv resulting in fewer escape
   *                           characters and problems such as "  " (2 spaces)
   *                           being interpretted properly. Array parameter
   *                           values support improved.
   *   @history 2009-11-19 Kris Becker - Made argc pass by reference since Qt's
   *                           QApplication/QCoreApplication requires it
   *   @history 2010-03-26 Sharmila Prasad - Remove the restriction of the
   *                           number of columns in the batchlist file to 10.
   *   @history 2010-05-28 Steven Lambright - History fails silently now
   *   @history 2010-07-20 Steven Lambright - Array format on the command line
   *                           is more tolerant to white space now.
   *   @history 2010-07-28 Steven Lambright - Fixed complicated escape sequence
   *                           cases on array format parsing that have existed
   *                           for a while now
   *   @history 2010-07-28 Christopher Austin - Fixed a -LAST issue causing
   *                           IsisAml to throw an incorrect exception.
   *   @history 2010-10-28 Mackenzie Boyd - Modified error messages in
   *                           LoadHistory()\
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *   @history 2011-08-31 Jai Rideout and Steven Lambright - Loading the
   *                           history with -last no longer causes us to call
   *                           PutAsString for parameters which match the
   *                           defaults. This fixes an issue with the new
   *                           spiceinit GUI with "WEB" added... -last would
   *                           always throw an error.
   *   @history 2011-09-21 Steven Lambright - When the -batchlist does not
   *                           understand a variable it would crash. This
   *                           prevented users from entering variables like
   *                           $base, $mro, etc. Now when -batchlist does not
   *                           understand a variable it preserves it in the
   *                           parameter list. Fixes #365.
   */

  class UserInterface : public IsisAml {
    public:
      UserInterface(const QString &xmlfile, int &argc, char *argv[]);
      ~UserInterface();

      /**
       * Indicates if the Isis Graphical User Interface is operating.
       *
       * @return bool
       */
      bool IsInteractive() {
        return p_gui != NULL;
      };

      /**
       * @return the Gui
       */
      Gui *TheGui() {
        return p_gui;
      };


      /**
       * Returns the size of the batchlist.  If there is no batchlist, it will
       * return 0
       *
       * @return int The size of the batchlist
       */
      int BatchListSize() {
        return p_batchList.size();
      };

      /**
       * Returns the parent id
       *
       * @return int The parent id
       */
      int ParentId() {
        return p_parentId;
      };

      /**
       * Returns true if the program should abort on error, and false if it
       * should continue
       *
       * @return bool True for abort, False for continue
       */
      bool AbortOnError() {
        return p_abortOnError;
      };

      void SaveHistory();
      void SetBatchList(int i);
      void SetErrorList(int i);

      bool GetInfoFlag();
      QString GetInfoFileName();

    private:
      std::vector<char *> p_cmdline; /**< This variable will contain argv.*/
      int p_parentId;               /**< This is a status to indicate if the GUI
                                        is running or not.*/

      void LoadCommandLine(int argc, char *argv[]);
      void LoadBatchList(const QString file);
      void LoadHistory(const QString file);
      void EvaluateOption(const QString name, const QString value);
      void GetNextParameter(unsigned int &curPos,
                            QString &name, std::vector<QString> &value);
      std::vector<QString> ReadArray(QString arrayString);

      //! Boolean value representing whether to abort or continue on error
      bool p_abortOnError;
      QString p_saveFile;        //!< FileName to save last history to
      QString p_progName;        //!< Name of program to run

      //!FileName to write batchlist line that caused error on
      QString p_errList;

      //!Vector of batchlist data
      std::vector<std::vector<QString> > p_batchList;

      bool p_interactive;  /**< Boolean value representing whether the
                                program is interactive or not.*/
      bool p_info;  //!< Boolean value representing if its in debug mode.
      QString p_infoFileName;  //!< FileName to save debugging info
      Gui *p_gui;                  //!< Pointer to the gui object
  };
};

#endif