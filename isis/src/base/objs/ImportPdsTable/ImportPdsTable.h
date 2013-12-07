#ifndef ImportPdsTable_h
#define ImportPdsTable_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
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

#include <QString>

#include <fstream>
#include <vector>
#include <string>

class QString;

namespace Isis {
  class Table;
  class TableField;
  class TableRecord;
  class PvlObject;
  /**
   * @brief Import a PDS table file with a label description
   *
   * This class ingests a PDS table and converts it to an ISIS Table
   * object. This class can handle importing PDS tables whose data is BINARY 
   * or ASCII format.
   *
   * The PDS label file should completely define the contents of a PDS
   * table with ASCII data or BINARY data. The location of the table data is 
   * determined from a label keyword, ^TABLE (or ^NAME_TABLE). This keyword 
   * will indicate the name of the file containing the table data and the start
   * record value where the table data begins. If the keyword only gives the 
   * start record, the table is attached to the file that contains the label.
   * If the keyword only gives a file name, that file only contains the data 
   * for the indicated table. PDS table files have the extension *.TAB if the 
   * table data is ASCII format and *.DAT if the data is BINARY. When 
   * constructing an ImportPdsTable object, programmers can provide a different
   * name for the PDS table file.
   *
   * <p>
   * @b NOTE: Depending on the INTERCHANGE_FORMAT value, programmers should 
   * choose the appropriate methods from this class.
   *
   * Example of PDS ASCII table import. Construct the ImportPdsTable object
   * with a label name, set the data type for the specified PDS columns, and 
   * import the PDS table to an Isis3 table with the comma separtated column
   * names.
   * <code>
   *   ImportPdsTable pdsTable(labelFileName, tableFileName);
   *   pdsTable.setType("ScetTimeClock", "CHARACTER");
   *   pdsTable.setType("ShutterStatus", "CHARACTER");
   *   pdsTable.setType("MirrorSin", "DOUBLE");
   *   pdsTable.setType("MirrorCos", "DOUBLE");
   *   Table isisTable = pdsTable.importTable("ScetTimeClock,ShutterStatus,MirrorSin,MirrorCos",
   *                                          "VIRHouseKeeping");
   * </code>
   * Example of PDS BINARY table import. If the default constructor is used, 
   * the load method must be called to pass in the label file name. The PDS 
   * table can then be imported and an Isis3 Table object is returned.
   * <code>
   *   ImportPdsTable pdsTable;
   *   load(labelFileName, "", pdsTableName);
   *   Table isisTable = pdsTable.importTable(isisTableName);
   * </code>
   * </p>
   * @ingroup Utility
   *
   * @author 2011-07-20 Kris Becker
   *
   * @internal 
   *   @history 2011-08-02 Kris Becker - Modified to ensure the proper size of strings are
   *                         exported to the TableRecord
   *   @history 2012-10-04 Jeannie Backer - Changed references to TableField
   *                           methods to lower camel case. Added and ordered
   *                           includes. Moved method implementation to cpp.
   *                           References #1169.
   *   @history 2012-11-21 Jeannie Backer - Added implementation for importing 
   *                           binary PDS tables. Changed method name from
   *                           "exportAsTable" to "importTable". Improved
   *                           unitTest coverage. References #700.
   *  
   *  
   * @todo The binary table import methods were written after the ascii table 
   *       import. The class should be better organized so that the programmer
   *       does not need to know which methods to call based on
   *       INTERCHANGE_FORMAT.  The program should take care of this
   *       internally.
   */
  class ImportPdsTable {
    public:
      ImportPdsTable();
      ImportPdsTable(const QString &pdsLabFile, 
                     const QString &pdsTabFile="",
                     const QString &pdsTableName="TABLE");
      ~ImportPdsTable();
  
      int columns() const;
      int rows() const;
  
      void load(const QString &pdsLabFile, const QString &pdsTabFile = "");
  
      bool hasColumn(const QString &colName) const;
      QString getColumnName(const unsigned int &index = 0, 
                                const bool &formatted = true) const;
      QStringList getColumnNames(const bool &formatted = true) const;
      QString getFormattedName(const QString &colname) const;
  
      QString getType(const QString &colName) const;
      bool setType(const QString &colName, const QString &dataType);
 
      Table importTable(const QString &isisTableName);
      Table importTable(const QString &colNames,
                        const QString &isisTableName);
      Table importTable(const QStringList &colNames,
                        const QString &isisTableName);
  
    private:
  
      struct ColumnDescr {
        QString m_name;       //!< Name of column
        int         m_colnum;     //!< Column number
        QString m_dataType;   //!< PDS table DATA_TYPE of column
        int         m_startByte;  //!< Starting byte of data
        int         m_numBytes;     //!< Number bytes in column
        int         m_itemBytes;  //!<
      };
  
      typedef std::vector<ColumnDescr> ColumnTypes;
      typedef std::vector<QString>     Columns;
      typedef std::vector<Columns>     Rows;
  
      void init();

      void loadLabel(const QString &labfile, QString &tblfile);
      void loadTable(const QString &tabfile);

      ColumnDescr getColumnDescription(PvlObject &colobj, int nth) const;
      ColumnDescr *findColumn(const QString &colName);
      const ColumnDescr *findColumn(const QString &colName) const;

      QString getColumnValue(const QString &tline,
                                 const ColumnDescr &cdesc) const;
      QString getGenericType(const QString &ttype) const;

      TableRecord makeRecord(const ColumnTypes &ctypes);
      TableField makeField(const ColumnDescr &cdesc);
      TableField makeFieldFromBinaryTable(const ColumnDescr &cdesc);
      void setPdsByteOrder(QString byteOrder);

      TableField &extract(const Columns &columns, const ColumnDescr &cdesc,
                          TableField &field) const;
      TableRecord &extract(const Columns &columns, const ColumnTypes &ctypes,
                           TableRecord &record) const;
      TableRecord extractBinary(char *rowBuffer, TableRecord &record) const;

      void fillTable(Table &table, const ColumnTypes &columns,
                     TableRecord &record) const;
      
      //private instance variables
      int         m_trows;      //!< Number rows in table according to label
      ColumnTypes m_coldesc;    //!< Column descriptions
      Rows        m_rows;       //!< Table data
      QString  m_pdsTableType;  //!< The INTERCHANGE_FORMAT value for the table.
      int      m_rowBytes;      //!< The number of bytes for one PDS table row.
      int      m_recordBytes;   //!< The number of bytes for one Isis table record.
      QString  m_tableName;     //!< The name of the PDS table object
      QString  m_pdsTableFile;  //!< The name of the file containing the table data.
      int      m_pdsTableStart; //!< The start byte of the PDS table data.
      QString  m_byteOrder;     /**< The byte order of the PDS table file, if 
                                     binary. Valid values are "MSB" or "LSB".*/

  };

}
#endif

