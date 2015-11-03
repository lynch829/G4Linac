#include "RunAction.hh"
#include "RunMessenger.hh"
#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include "G4IAEAphspWriter.hh"
#include <sys/time.h>
RunAction::RunAction()
{
messenger = new RunMessenger(this); 
}


RunAction::~RunAction()
{
 delete messenger;
}


void RunAction::BeginOfRunAction(const G4Run* aRun)
{
m1=msecs();
G4IAEAphspWriter* IAEAWriter = G4IAEAphspWriter::GetInstance();
IAEAWriter->SetZStop(this->PhspZstop); 
IAEAWriter->SetFileName(this->PhspName);
G4IAEAphspWriter::GetInstance()->BeginOfRunAction(aRun);
G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
G4cout << "### Run " << aRun->GetNumberOfEventToBeProcessed()<< " evenement." << G4endl; 
}


void RunAction::EndOfRunAction(const G4Run* aRun)
{

G4IAEAphspWriter* IAEAWriter = G4IAEAphspWriter::GetInstance();
IAEAWriter->EndOfRunAction(aRun);

m2=msecs();
G4cout << "### Run time " << m2-m1<< " s." << G4endl; 
  }
 void RunAction::SetPhspZstop(G4double PhspZstop)
{ 
this->PhspZstop=PhspZstop;
}
 void RunAction::SetPhspName(const G4String& PhspName)
{ 
this->PhspName=PhspName;
}

double RunAction::msecs() /* milliseconds */
{
struct timeval tv;
struct timezone tz;
tz.tz_minuteswest=-120;
tz.tz_dsttime=0;
gettimeofday(&tv, &tz);
return 0.000001*((long long)tv.tv_sec*1000000+tv.tv_usec);
 
}

