#include "ScenePropertiesWidget.h"

#include "SceneModel.h"
#include "CurveModel.h"
#include "StepCurveModel.h"

#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QTableView>
#include <QAbstractTableModel>
#include <QHeaderView>
#include <QLabel>

class CurveTableModel : public QAbstractTableModel
{
public:
    CurveTableModel()
      : m_selectionModel(new QItemSelectionModel(this))
    {}

    ~CurveTableModel()
    {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const
    {
        Q_UNUSED(parent);
        return m_curves.size();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const
    {
        Q_UNUSED(parent);
        return 3;
    }

    Qt::ItemFlags flags(const QModelIndex& index) const
    {
//        NoItemFlags = 0,
//        ItemIsSelectable = 1,
//        ItemIsEditable = 2,
//        ItemIsDragEnabled = 4,
//        ItemIsDropEnabled = 8,
//        ItemIsUserCheckable = 16,
//        ItemIsEnabled = 32,
//        ItemIsTristate = 64,
//        ItemNeverHasChildren = 128

        Qt::ItemFlags itemFlags = Qt::NoItemFlags;
        itemFlags |= Qt::ItemIsEnabled;
        itemFlags |= Qt::ItemIsEditable;

        if (index.column() == 0)
            itemFlags |= Qt::ItemIsSelectable;

        return itemFlags;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
    {
        std::shared_ptr<CurveModelAbs> curve = m_curves[index.row()];
        Q_ASSERT(curve);

        std::shared_ptr<CurveModel> splineCurve = CurveModelAbs::getAsSplineCurve(curve);
        std::shared_ptr<StepCurveModel> stepCurve = CurveModelAbs::getAsStepCurve(curve);

        if (role == Qt::DisplayRole)
        {
            switch (index.column())
            {
            case 0: return curve->name();
            case 1: return splineCurve ? splineCurve->valueRange().min : (stepCurve ? 0.0f : -1.0f);
            case 2: return splineCurve ? splineCurve->valueRange().max : (stepCurve ? stepCurve->options().size() : -1.0f);
            default: break;
            }
            return "Data";
        }

        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
        if (role == Qt::DisplayRole)
        {
            // Only column headers
            if (orientation == Qt::Horizontal)
            {
                switch (section)
                {
                case 0: return "Curve";
                case 1: return "Min";
                case 2: return "Max";
                default : break;
                }
            }
        }

        return QVariant();
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole)
    {
        std::shared_ptr<CurveModelAbs> curve = m_curves[index.row()];
        Q_ASSERT(curve);

        std::shared_ptr<CurveModel> splineCurve = CurveModelAbs::getAsSplineCurve(curve);
        std::shared_ptr<StepCurveModel> stepCurve = CurveModelAbs::getAsStepCurve(curve);

        if (role == Qt::EditRole)
        {
            switch (index.column())
            {
            case 0:
            {
                QString name = value.toString();
                if ((name.compare(curve->name()) != 0) && !name.isEmpty())
                {
                    curve->setName(name);
                    return true;
                }
                break;
            }
            case 1:
            {
                bool ok = false;
                double start = value.toDouble(&ok);
                if (splineCurve)
                    if (ok && start < splineCurve->valueRange().max)
                    {
                        splineCurve->setValueRange(RangeF(start, splineCurve->valueRange().max));
                        return true;
                    }
                break;
            }
            case 2:
            {
                bool ok = false;
                double end = value.toDouble(&ok);
                if (splineCurve)
                {
                    if (ok && end > splineCurve->valueRange().min)
                    {
                        splineCurve->setValueRange(RangeF(splineCurve->valueRange().min, end));
                        return true;
                    }
                }
                else if (stepCurve)
                {
                    if (ok && end > 0)
                    {
                        StepCurveModel::Options newOptions;
                        for (int i = 0; i < end; ++i)
                        {
                            char buf[16];
                            sprintf(buf, "%d", i);
                            newOptions.insert(i, buf);
                        }
                        stepCurve->setOptions(newOptions);
                        return true;
                    }
                }
                break;
            }
            default:
                break;
            }
        }

        return false;
    }

    QItemSelectionModel* getSelectionModel() const
    {
        return m_selectionModel;
    }

public slots:
    void addCurve(std::shared_ptr<CurveModelAbs> curve)
    {
        if (m_curves.contains(curve))
            return;

        beginInsertRows(QModelIndex(), m_curves.size(), m_curves.size());
        m_curves.push_back(curve);
        endInsertRows();
    }

    void removeCurve(std::shared_ptr<CurveModelAbs> curve)
    {
        if (!m_curves.contains(curve))
            return;

        int index = m_curves.indexOf(curve);
        if (index == -1)
            return;

        beginRemoveRows(QModelIndex(), index, index);
        m_curves.removeAt(index);
        endRemoveRows();
    }

    void clearCurves()
    {
        beginResetModel();
        m_curves.clear();
        endResetModel();
    }

    void selectCurve(std::shared_ptr<CurveModelAbs> curve)
    {
        int index = m_curves.indexOf(curve);
        if (index == -1)
        {
            qWarning() << "Unknown curve selected";
            return;
        }

        QModelIndex rowIndex = QAbstractItemModel::createIndex(index, 0);
        if (!m_selectionModel->isSelected(rowIndex))
        {
            m_selectionModel->select(QAbstractItemModel::createIndex(index, 0), QItemSelectionModel::Select);
        }
    }

    void deselectCurve(std::shared_ptr<CurveModelAbs> curve)
    {
        int index = m_curves.indexOf(curve);
        if (index == -1)
        {
            qWarning() << "Unknown curve selected";
            return;
        }

        QModelIndex rowIndex = QAbstractItemModel::createIndex(index, 0);
        if (!m_selectionModel->isSelected(rowIndex))
        {
            m_selectionModel->select(QAbstractItemModel::createIndex(index, 0), QItemSelectionModel::Deselect);
        }
    }

    void tableRowsSelected(const QItemSelection& selected, const QItemSelection& deselected)
    {
        qDebug() << "tableRowsSelected";

        for (auto index : selected.indexes())
        {
            if (index.column() == 0)
            {
                qDebug() << "Curve row selected:" << index.row();

                Q_ASSERT(index.row() < m_curves.size());
                m_curves[index.row()]->setSelected(true);
            }
        }
        for (auto index : deselected.indexes())
        {
            if (index.column() == 0)
            {
                qDebug() << "Curve row deselected:" << index.row();

                Q_ASSERT(index.row() < m_curves.size());
                m_curves[index.row()]->setSelected(false);
            }
        }
    }

private:
    QList<std::shared_ptr<CurveModelAbs>> m_curves;

    QItemSelectionModel* m_selectionModel;
};

ScenePropertiesWidget::ScenePropertiesWidget(QWidget * parent)
:	QWidget(parent),
    m_gridLayout(new QGridLayout(this))
{
    QLabel* beatOffsetLabel = new QLabel("1st beat", this);
    m_beatOffsetSpinner = new QDoubleSpinBox(this);
    m_beatOffsetSpinner->setSuffix(" s");
    m_beatOffsetSpinner->setRange(-60, 60);
    m_beatOffsetSpinner->setSingleStep(0.2);
    m_beatOffsetSpinner->setValue(0);
    m_beatOffsetSpinner->setEnabled(false);
    m_gridLayout->addWidget(beatOffsetLabel, 0, 0);
    m_gridLayout->addWidget(m_beatOffsetSpinner, 0, 1);

    QLabel* bpmLabel = new QLabel("Bpm", this);
    m_bpmSpinner = new QDoubleSpinBox(this);
    m_bpmSpinner->setSuffix(" bpm");
    m_bpmSpinner->setRange(10, 360);
    m_bpmSpinner->setSingleStep(0.2);
    m_bpmSpinner->setValue(80);
    m_bpmSpinner->setEnabled(false);
    m_gridLayout->addWidget(bpmLabel, 1, 0);
    m_gridLayout->addWidget(m_bpmSpinner, 1, 1);

    QLabel* timeLineLengthLabel = new QLabel("Timeline", this);
    m_timeLineLength = new QDoubleSpinBox(this);
    m_timeLineLength->setSuffix(" s");
    m_timeLineLength->setRange(10, 1000);
    m_timeLineLength->setSingleStep(1);
    m_timeLineLength->setValue(240);
    m_timeLineLength->setEnabled(false);
    m_gridLayout->addWidget(timeLineLengthLabel, 2, 0);
    m_gridLayout->addWidget(m_timeLineLength, 2, 1);

    m_curveTableModel = new CurveTableModel;
    m_curveTable = new QTableView(this);
    m_curveTable->setModel(m_curveTableModel);
    m_curveTable->setSelectionModel(m_curveTableModel->getSelectionModel());
    // Make columns stretch to available space
    m_curveTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_gridLayout->addWidget(m_curveTable, 3, 0, 1, 2);

    // Listen to table selection changes
    connect(m_curveTable->selectionModel(), &QItemSelectionModel::selectionChanged,
            m_curveTableModel, &CurveTableModel::tableRowsSelected);

    this->setLayout(m_gridLayout);
}

ScenePropertiesWidget::~ScenePropertiesWidget()
{
}

void ScenePropertiesWidget::setSceneModel(std::shared_ptr<SceneModel> sceneModel)
{
    if (m_sceneModel == sceneModel)
    {
        return;
    }

    if (m_sceneModel)
    {
        // Disconnect from old scene
        disconnect(m_beatOffsetSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBeatOffset(double)));
        disconnect(m_bpmSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBpm(double)));
        disconnect(m_timeLineLength, SIGNAL(valueChanged(double)), this, SLOT(setTimeLineLength(double)));
        disconnect(m_sceneModel.get(), &SceneModel::curveAdded, m_curveTableModel, &CurveTableModel::addCurve);
        disconnect(m_sceneModel.get(), &SceneModel::curveRemoved, m_curveTableModel, &CurveTableModel::removeCurve);
        disconnect(m_sceneModel.get(), &SceneModel::curveSelected, m_curveTableModel, &CurveTableModel::selectCurve);
        disconnect(m_sceneModel.get(), &SceneModel::curveDeselected, m_curveTableModel, &CurveTableModel::deselectCurve);

        // Clear curve table
        m_curveTableModel->clearCurves();

        // Disable beat offset and bpm spinners
        m_beatOffsetSpinner->setEnabled(false);
        m_bpmSpinner->setEnabled(false);
        m_timeLineLength->setEnabled(false);
    }

    // Change the model
    m_sceneModel = sceneModel;

    if (m_sceneModel)
    {
        // Connect to new scene
        connect(m_beatOffsetSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBeatOffset(double)));
        connect(m_bpmSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBpm(double)));
        connect(m_timeLineLength, SIGNAL(valueChanged(double)), this, SLOT(setTimeLineLength(double)));
        connect(m_sceneModel.get(), &SceneModel::curveAdded, m_curveTableModel, &CurveTableModel::addCurve);
        connect(m_sceneModel.get(), &SceneModel::curveRemoved, m_curveTableModel, &CurveTableModel::removeCurve);
        connect(m_sceneModel.get(), &SceneModel::curveSelected, m_curveTableModel, &CurveTableModel::selectCurve);
        connect(m_sceneModel.get(), &SceneModel::curveDeselected, m_curveTableModel, &CurveTableModel::deselectCurve);

        // Enable and initialize beat offset spinner
        m_beatOffsetSpinner->setValue(m_sceneModel->beatOffset());
        m_beatOffsetSpinner->setEnabled(true);

        // Enable and initialize bpm spinner
        m_bpmSpinner->setValue(m_sceneModel->bpm());
        m_bpmSpinner->setEnabled(true);

        // Enable and initialize bpm spinner
        m_timeLineLength->setValue(m_sceneModel->timeRange().max);
        m_timeLineLength->setEnabled(true);

        // Add existing curves to the curve table
        for (auto curve : m_sceneModel->curves())
        {
            m_curveTableModel->addCurve(curve);
            if (curve->isSelected())
                m_curveTableModel->selectCurve(curve);
        }
    }
}

void ScenePropertiesWidget::setTimeLineLength(double length)
{
    if (!m_sceneModel)
        return;

    m_sceneModel->setTimeRange(RangeF(0, static_cast<float>(length)));
}

