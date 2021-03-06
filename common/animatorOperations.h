#ifndef ANIMATOROPERATIONS_H
#define ANIMATOROPERATIONS_H



void initializeAnimator();
void clearAnimator();


//async
void addSkelKeyframe();
void addCageKeyframe();
void setSkelKeyframe(unsigned long nodeIndex);
void setCageKeyframe(unsigned long nodeIndex);
void loadSkelAnimationFromFile();
void loadCageAnimationFromFile();
void saveSkelAnimationToFile();
void saveCageAnimationToFile();
void setNextSkelKeyframe();
void setNextCageKeyframe();
void deleteSkelKeyframe(int index);
void deleteCageKeyframe(int index);
void editSkelKeyframeTime(int index);
void editCageKeyframeTime(int index);
void quickLoadSkelAnimation(const char * filename);
void quickLoadCageAnimation(const char * filename);

#endif // ANIMATOROPERATIONS_H
