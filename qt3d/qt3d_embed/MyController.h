
#include <QObject>

class QMainWindow;

class MyController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMainWindow* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(float time READ time WRITE setTime NOTIFY timeChanged)

public:
    MyController(QObject *parent = 0)
        : QObject(parent)
        , m_target(Q_NULLPTR)
    {
    }

    void setTarget(QMainWindow* target)
    {
        if (m_target != target) {
            m_target = target;
            emit targetChanged();
        }
    }

    QMainWindow* target() const { return m_target; }

    void setTime(float time)
    {
      m_time = time;
      if((int)m_time % 60 == 0) {
        drawUpdate();
        emit timeChanged();
      }
    }

    float time() const { return m_time; }

    void drawUpdate();
  
signals:
    void targetChanged();
    void timeChanged();

private:
    QMainWindow* m_target;
    float m_time;
};
