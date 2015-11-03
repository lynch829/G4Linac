

#ifndef StackingAction_H
#define StackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"
#include "G4ThreeVector.hh"
class StackingMessenger;
class G4Track;


class StackingAction : public G4UserStackingAction
{
  public:
    StackingAction();
    virtual ~StackingAction();

  public:
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();
G4bool GAMMATHEC(const G4Track * aTrack);
  static void SetIsActive(G4bool);
  static G4bool fActive;
  // Accessors
  static G4bool GetIsActive();

  private:

    G4int stage;

  
protected:
StackingMessenger* messenger;

};

#endif

