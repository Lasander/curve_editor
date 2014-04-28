#include "ScenePropertiesWidget.h"

#include "SceneModel.h"
#include "CurveModel.h"

#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QTableView>
#include <QAbstractTableModel>

class CurveTableModel : public QAbstractTableModel
{
public:
    CurveTableModel()
    {}

    ~CurveTableModel()
    {}

    int rowCount(const QModelIndex & parent = QModelIndex()) const
    {
        return m_curves.size();
    }

    int columnCount(const QModelIndex & parent = QModelIndex()) const
    {
        return 2;
    }

    Qt::ItemFlags flags(const QModelIndex & index) const
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

        return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const
    {
        if (role == Qt::DisplayRole)
        {
            std::shared_ptr<CurveModel> curve = m_curves[index.row()];

            switch (index.column())
            {
            case 0: return curve->valueRange().min; //QString::number(curve->valueRange().min, 'f', 1);
            case 1: return curve->valueRange().max; //QString::number(curve->valueRange().max, 'f', 1);
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
            if (orientation == Qt::Vertical)
            {
                return m_curves[section]->name();
            }
            else
            {
                switch (section)
                {
                case 0: return "Min";
                case 1: return "Max";
                default : break;
                }
            }
            return "Header";
        }

        return QVariant();
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole)
    {
        if (role == Qt::EditRole)
        {
            std::shared_ptr<CurveModel> curve = m_curves[index.row()];
            switch (index.column())
            {
            case 0:
            {
                bool ok = false;
                double start = value.toDouble(&ok);
                if (ok && start < curve->valueRange().max)
                {
                    curve->setValueRange(RangeF(start, curve->valueRange().max));
                }
                break;
            }
            case 1:
            {
                bool ok = false;
                double end = value.toDouble(&ok);
                if (ok && end > curve->valueRange().min)
                {
                    curve->setValueRange(RangeF(curve->valueRange().min, end));
                }
                break;
            }
            default: break;
            }
        }

    }

public slots:
    void addCurve(std::shared_ptr<CurveModel> curve)
    {
        if (m_curves.contains(curve))
            return;

        beginInsertRows(QModelIndex(), m_curves.size(), m_curves.size());
        m_curves.push_back(curve);
        endInsertRows();
    }

    void removeCurve(std::shared_ptr<CurveModel> curve)
    {
        if (!m_curves.contains(curve))
            return;

        int index = m_curves.indexOf(curve);
        if (index == -1)
            return

        beginRemoveRows(QModelIndex(), index, index);
        m_curves.removeAt(index);
        endRemoveRows();
    }

    void clearCurves()
    {
        m_curves.clear();
    }

    void selectCurve(std::shared_ptr<CurveModel> curve)
    {

    }

    void deselectCurve(std::shared_ptr<CurveModel> curve)
    {

    }

private:
    QList<std::shared_ptr<CurveModel>> m_curves;
};

ScenePropertiesWidget::ScenePropertiesWidget(QWidget * parent)
:	QWidget(parent),
    m_gridLayout(new QGridLayout())
{
    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    vboxLayout->addLayout(m_gridLayout);

    m_beatOffsetSpinner = new QDoubleSpinBox(this);
    m_beatOffsetSpinner->setSuffix(" s");
    m_beatOffsetSpinner->setRange(-60, 60);
    m_beatOffsetSpinner->setSingleStep(0.2);
    m_beatOffsetSpinner->setValue(0);
    m_beatOffsetSpinner->setEnabled(false);
    vboxLayout->addWidget(m_beatOffsetSpinner);

    m_bpmSpinner= new QDoubleSpinBox(this);
    m_bpmSpinner->setSuffix(" bpm");
    m_bpmSpinner->setRange(10, 360);
    m_bpmSpinner->setSingleStep(0.2);
    m_bpmSpinner->setValue(80);
    m_bpmSpinner->setEnabled(false);
    vboxLayout->addWidget(m_bpmSpinner);


    m_curveTableModel = new CurveTableModel;
    m_curveTable = new QTableView(this);
    m_curveTable->setModel(m_curveTableModel);
    vboxLayout->addWidget(m_curveTable);

    this->setLayout(vboxLayout);
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
        disconnect(m_beatOffsetSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBeatOffset(double)));
        disconnect(m_bpmSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBpm(double)));

        disconnect(m_sceneModel.get(), &SceneModel::curveAdded, m_curveTableModel, &CurveTableModel::addCurve);
        disconnect(m_sceneModel.get(), &SceneModel::curveRemoved, m_curveTableModel, &CurveTableModel::removeCurve);
        m_curveTableModel->clearCurves();

        m_beatOffsetSpinner->setEnabled(false);
        m_bpmSpinner->setEnabled(false);
    }

    m_sceneModel = sceneModel;

    if (m_sceneModel)
    {
        connect(m_beatOffsetSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBeatOffset(double)));
        connect(m_bpmSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBpm(double)));

        m_beatOffsetSpinner->setValue(m_sceneModel->beatOffset());
        m_beatOffsetSpinner->setEnabled(true);

        m_bpmSpinner->setValue(m_sceneModel->bpm());
        m_bpmSpinner->setEnabled(true);

        connect(m_sceneModel.get(), &SceneModel::curveAdded, m_curveTableModel, &CurveTableModel::addCurve);
        connect(m_sceneModel.get(), &SceneModel::curveRemoved, m_curveTableModel, &CurveTableModel::removeCurve);
        for (auto curve : m_sceneModel->curves())
            m_curveTableModel->addCurve(curve);
    }

}
