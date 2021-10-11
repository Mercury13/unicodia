#pragma once

#include <QTableView>

class WrapAroundTable : public QTableView
{
    using Super = QTableView;
protected:
    void keyPressEvent(QKeyEvent*) override;
public:
    using Super::Super;
};
