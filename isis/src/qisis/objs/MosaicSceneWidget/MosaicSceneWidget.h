#ifndef MosaicSceneWidget_H
#define MosaicSceneWidget_H

#include <QWidget>

template <typename A> class QList;
class QGraphicsPolygonItem;
class QGraphicsRectItem;
class QGraphicsScene;
class QMenu;
class QProgressBar;
class QRubberBand;
class QStatusBar;
class QToolBar;
class QToolButton;

namespace Isis {
  class CubeDisplayProperties;
  class MosaicGraphicsView;
  class MosaicSceneItem;
  class MosaicTool;
  class ProgressBar;
  class Projection;
  class PvlGroup;
  class PvlObject;
  class ToolPad;

  /**
   * @brief This widget encompasses the entire mosaic scene and is
   *        what you want to use from an application's point of view
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2010-05-10 Christopher Austin - Added cnet connectivity
   *                           functionality and fixed a few design issues
   *   @history 2011-04-01 Steven Lambright - Separated this class from the
   *                           MosaicWidget class.
   *   @history 2011-05-10 Steven Lambright - Reduced unnecessary code, fixed
   *                           toolTips to work on everything (not just
   *                           cubes).
   *   @history 2011-05-17 Steven Lambright - More robust createInitialProj
   *   @history 2011-05-17 Steven Lambright - Target radii recalculated when
   *                           the user specifies a map file, if they
   *                           are missing.
   *   @history 2011-05-20 Steven Lambright - Improved error handling when
   *                           reprojecting.
   *   @history 2011-07-29 Steven Lambright - Z-ordering is now saved and
   *                           restored in the project files. references #275
   *   @history 2011-08-12 Steven Lambright - Added export options,
   *                           references #342
   *   @history 2011-08-29 Steven Lambright - Re-worded export file list option,
   *                           references #342
   *   @history 2011-09-27 Steven Lambright - Improved user documentation
   *   @history 2011-11-04 Steven Lambright - Added the zoom factor and
   *                           scroll bar position to the project file.
   *                           References #542.
   *   @history 2011-11-04 Steven Lambright - The mouse wheel events no longer
   *                           cause panning. The qt code for
   *                           QAbstractGraphicsView was looking at the event's
   *                           accepted state. This being fixed means the mouse
   *                           wheel can be used for zooming! Also added
   *                           getViewActions in order to allow the zooming key
   *                           shortcuts from the zoom tool to take effect when
   *                           the zoom tool wasn't active.
   *   @history 2012-06-20 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2012-07-26 Kimberly Oyama - Updated the help documentation for
   *                           the grid tool to include tips for the options
   *                           dialog and the new 'Auto Grid' functionality.
   *                           References #604.
   *   @history 2012-10-11 Debbie A. Cook, Updated to use new Target class.  References Mantis tickets 
   *                           #775 and #1114.
   *   @history 2013-01-31 Steven Lambright - Fixed a problem caused by #1312 - when the minimum
   *                           longitude wasn't defined in the map file, the one generated by qmos
   *                           was invalid. Fixes #1406.
   *   @history 2012-12-21 Steven Lambright - Renamed askNewProjection() to
   *                           configProjectionParameters() and upgraded it's functionality to view
   *                           and edit the current projection. Fixes #1034.
   */
  class MosaicSceneWidget : public QWidget {
      Q_OBJECT

    public:
      MosaicSceneWidget(QStatusBar *status, QWidget *parent = 0);
      virtual ~MosaicSceneWidget();

      MosaicGraphicsView *getView() const {
        return m_graphicsView;
      }

      QGraphicsScene *getScene() const {
        return m_graphicsScene;
      }

      Projection *getProjection() const {
        return m_projection;
      }

      QList<MosaicSceneItem *> allMosaicSceneItems() {
        return *m_mosaicSceneItems;
      }

      void addTo(QMenu *menu);
      void addTo(ToolPad *toolPad);
      void addToPermanent(QToolBar *toolBar);
      void addTo(QToolBar *toolBar);

      bool cubesSelectable() const {
        return m_cubesSelectable;
      }

      void enableRubberBand(bool);
      void blockSelectionChange(bool);

      bool userHasTools() const {
        return m_userToolControl;
      }

      QProgressBar *getProgress();
      PvlObject toPvl() const;
      void fromPvl(const PvlObject &);
      void preloadFromPvl(const PvlObject &);

      QRectF cubesBoundingRect() const;
      QStringList cubeFileNames();
      QList<CubeDisplayProperties *> cubeDisplays();

      QList<QAction *> getExportActions();
      QList<QAction *> getViewActions();

      static QWidget *getControlNetHelp(QWidget *cnetToolContainer = NULL);
      static QWidget *getGridHelp(QWidget *gridToolContainer = NULL);
      static QWidget *getLongHelp(QWidget *mosaicSceneContainer = NULL);
      static QWidget *getMapHelp(QWidget *mapContainer = NULL);
      static QWidget *getPreviewHelp(QWidget *worldViewContainer = NULL);

    signals:
      void mouseEnter();
      void mouseMove(QPointF);
      void mouseLeave();
      void mouseDoubleClick(QPointF);
      void mouseButtonPress(QPointF, Qt::MouseButton s);
      void mouseButtonRelease(QPointF, Qt::MouseButton s);
      void mouseWheel(QPointF, int delta);
      void projectionChanged(Projection *);
      void rubberBandComplete(QRectF r, Qt::MouseButton s);
      void visibleRectChanged(QRectF);

      void cubesChanged();

    public slots:
      void addCubes(QList<CubeDisplayProperties *>);
      void refit();
      void setCubesSelectable(bool);
      void setProjection(Projection *);
      void setOutlineRect(QRectF);

    private slots:
      void exportView();
      void saveList();

      void removeMosItem(QObject *);

      void moveDownOne();
      void moveToBottom();
      void moveUpOne();
      void moveToTop();
      void fitInView();

      void onSelectionChanged();

      void configProjectionParameters();
      void quickConfigProjectionParameters();
      void sendVisibleRectChanged();

    protected:
      virtual bool eventFilter(QObject *obj, QEvent *ev);

    private:
      void setProjection(const PvlGroup &);
      MosaicSceneItem *addCube(CubeDisplayProperties *);
      void createReferenceFootprint();
      void reprojectItems();
      qreal maximumZ();
      qreal minimumZ();
      void recalcSceneRect();

      MosaicSceneItem *getNextItem(MosaicSceneItem *item, bool up);

      PvlGroup createInitialProjection(CubeDisplayProperties *cube);

      MosaicSceneItem *cubeToMosaic(CubeDisplayProperties *);

      QList<CubeDisplayProperties *> getSelectedCubes() const;

      static bool zOrderGreaterThan(MosaicSceneItem *first,
                                    MosaicSceneItem *second);

      bool m_cubesSelectable;
      bool m_customRubberBandEnabled;
      QRubberBand *m_customRubberBand;
      QPoint *m_rubberBandOrigin;
      QGraphicsScene *m_graphicsScene; //!< The graphics scene that makes up this widget.
      MosaicGraphicsView *m_graphicsView; //!< The graphics view
      Projection *m_projection; //!< The current projection type.
      QGraphicsPolygonItem *m_projectionFootprint;
      QList<MosaicSceneItem *> *m_mosaicSceneItems;
      QGraphicsRectItem *m_outlineRect;

      QToolButton *m_mapButton;
      QAction *m_quickMapAction;

      QList<MosaicTool *> *m_tools;
      ProgressBar *m_progress;

      bool m_userToolControl;
      bool m_ownProjection;
  };
}

#endif

