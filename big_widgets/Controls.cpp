#include "Controls.h"

#include <git.h>
#include <GitQlientStyles.h>

#include <QApplication>
#include <QToolButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>

Controls::Controls(const QSharedPointer<Git> &git, QWidget *parent)
   : QFrame(parent)
   , mGit(git)
   , mHome(new QToolButton())
   , mBlame(new QToolButton())
   , mPullBtn(new QToolButton())
   , mPushBtn(new QToolButton())
   , mStashBtn(new QToolButton())
   , mRefreshBtn(new QToolButton())
{
   mHome->setIcon(QIcon(":/icons/git_orange"));
   mHome->setIconSize(QSize(22, 22));
   mHome->setText("Repo view");
   mHome->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

   mBlame->setIcon(QIcon(":/icons/blame"));
   mBlame->setIconSize(QSize(22, 22));
   mBlame->setText("Blame");
   mBlame->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

   const auto menu = new QMenu(mPullBtn);

   auto action = menu->addAction(tr("Fetch all"));
   connect(action, &QAction::triggered, this, &Controls::fetchAll);

   action = menu->addAction(tr("Pull"));
   connect(action, &QAction::triggered, this, &Controls::pullCurrentBranch);
   mPullBtn->setDefaultAction(action);

   action = menu->addAction(tr("Prune"));
   connect(action, &QAction::triggered, this, &Controls::pruneBranches);
   menu->addSeparator();

   mPullBtn->setMenu(menu);
   mPullBtn->setIconSize(QSize(22, 22));
   mPullBtn->setText(tr("Pull"));
   mPullBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
   mPullBtn->setPopupMode(QToolButton::MenuButtonPopup);
   mPullBtn->setIcon(QIcon(":/icons/git_pull"));

   mPushBtn->setIcon(QIcon(":/icons/git_push"));
   mPushBtn->setIconSize(QSize(22, 22));
   mPushBtn->setText(tr("Push"));
   mPushBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

   const auto stashMenu = new QMenu(mStashBtn);

   action = stashMenu->addAction(tr("Push"));
   connect(action, &QAction::triggered, this, &Controls::stashCurrentWork);

   // mStashBtn->setDefaultAction(action);
   action = stashMenu->addAction(tr("Pop"));
   connect(action, &QAction::triggered, this, &Controls::popStashedWork);

   mStashBtn->setMenu(stashMenu);
   mStashBtn->setIcon(QIcon(":/icons/git_stash"));
   mStashBtn->setIconSize(QSize(22, 22));
   mStashBtn->setText(tr("Stash"));
   mStashBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
   mStashBtn->setPopupMode(QToolButton::MenuButtonPopup);

   mRefreshBtn->setIcon(QIcon(":/icons/refresh"));
   mRefreshBtn->setIconSize(QSize(22, 22));
   mRefreshBtn->setText(tr("Refresh"));
   mRefreshBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

   const auto verticalFrame = new QFrame();
   verticalFrame->setObjectName("orangeSeparator");

   const auto verticalFrame2 = new QFrame();
   verticalFrame2->setObjectName("orangeSeparator");

   const auto hLayout = new QHBoxLayout(this);
   hLayout->setContentsMargins(10, 10, 10, 10);
   hLayout->addStretch();
   hLayout->addWidget(mHome);
   hLayout->addWidget(mBlame);
   hLayout->addWidget(verticalFrame);
   hLayout->addWidget(mPullBtn);
   hLayout->addWidget(mPushBtn);
   hLayout->addWidget(mStashBtn);
   hLayout->addWidget(verticalFrame2);
   hLayout->addWidget(mRefreshBtn);
   hLayout->addStretch();

   connect(mHome, &QToolButton::clicked, this, &Controls::signalGoBack);
   connect(mBlame, &QToolButton::clicked, this, &Controls::signalGoBlame);
   connect(mPushBtn, &QToolButton::clicked, this, &Controls::pushCurrentBranch);
   connect(mRefreshBtn, &QToolButton::clicked, this, &Controls::signalRepositoryUpdated);

   enableButtons(false);
}

void Controls::enableButtons(bool enabled)
{
   mHome->setEnabled(enabled);
   mPullBtn->setEnabled(enabled);
   mPushBtn->setEnabled(enabled);
   mStashBtn->setEnabled(enabled);
   mRefreshBtn->setEnabled(enabled);
}

void Controls::pullCurrentBranch()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   const auto ret = mGit->pull();
   QApplication::restoreOverrideCursor();

   if (ret.success)
      emit signalRepositoryUpdated();
   else
      QMessageBox::critical(this, tr("Error while pulling"), ret.output.toString());
}

void Controls::fetchAll()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   const auto ret = mGit->fetch();
   QApplication::restoreOverrideCursor();

   if (ret)
      emit signalRepositoryUpdated();
}

void Controls::pushCurrentBranch()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   const auto ret = mGit->push();
   QApplication::restoreOverrideCursor();

   if (ret.success)
      emit signalRepositoryUpdated();
   else
      QMessageBox::critical(this, tr("Error while pushing"), ret.output.toString());
}

void Controls::stashCurrentWork()
{
   const auto ret = mGit->stash();

   if (ret)
      emit signalRepositoryUpdated();
}

void Controls::popStashedWork()
{
   const auto ret = mGit->pop();

   if (ret.success)
      emit signalRepositoryUpdated();
   else
      QMessageBox::critical(this, tr("Error while stash pop"), ret.output.toString());
}

void Controls::pruneBranches()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   const auto ret = mGit->prune();
   QApplication::restoreOverrideCursor();

   if (ret.success)
      emit signalRepositoryUpdated();
}