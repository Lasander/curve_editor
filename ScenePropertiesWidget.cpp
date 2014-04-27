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
        return 4;
    }

    int columnCount(const QModelIndex & parent = QModelIndex()) const
    {
        return 2;
    }

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const
    {
        if (role == Qt::DisplayRole)
            return "Data";

        return QVariant();
    }

public slots:
    void addCurve(std::shared_ptr<CurveModel> curve)
    {

    }

    void removeCurve(std::shared_ptr<CurveModel> curve)
    {

    }

    void selectCurve(std::shared_ptr<CurveModel> curve)
    {

    }

    void deselectCurve(std::shared_ptr<CurveModel> curve)
    {

    }

private:

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


    m_curveTableModel = new CurveTableModel();
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
//            const int numberOfCurves = m_gridLayout->count();
//            for (int i = 0; i < numberOfCurves; ++i)
//            {
//                m_gridLayout->removeItem(m_gridLayout->itemAt(i));
//            }

        disconnect(m_beatOffsetSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBeatOffset(double)));
        disconnect(m_bpmSpinner, SIGNAL(valueChanged(double)), m_sceneModel.get(), SLOT(setBpm(double)));
//            disconnect(m_sceneModel.get(), &SceneModel::curveAdded, this, &ScenePropertiesWidget::addCurve);

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

//            connect(m_sceneModel.get(), &SceneModel::curveAdded, this, &ScenePropertiesWidget::addCurve);
//            for (auto curve : m_sceneModel->curves())
//                addCurve(curve);
    }

}

void ScenePropertiesWidget::addCurve(std::shared_ptr<CurveModel> /*curve*/)
{


    // TODO:
//        QLabel* nameLabel = new QLabel(curve->name());
//        m_gridLayout->addWidget(nameLabel, m_gridLayout->rowCount(), 0);
}

void ScenePropertiesWidget::removeCurve(std::shared_ptr<CurveModel> /*curve*/)
{
    // TODO:
}
