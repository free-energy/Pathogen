#include "PathogenSynth.h"
#include "IPlug_include_in_plug_src.h"
#include "resource.h"

#include "IControl.h"
#include "IKeyboardControl.h"

#include "Wavetable.h"
#include "WaveRIFFParser.h"

#include "IWaveFormDisplay.h"
#include "IWaveformDisplayInteractive.h"

const int kNumPrograms = 8;

#define PITCH 440.
#define TABLE_SIZE 4096

#ifndef M_PI
#define M_PI 3.14159265
#endif

#define GAIN_FACTOR 0.2;

enum EParams
{
	kAttack = 0,
	kDecay,
	kSustain,
	kRelease,
	kSelectFile,

	kOsc1StartPoint,
	kOsc1LoopPoint,
	kOsc1EndPoint,

	kOsc1LoopMode,
	kOsc1Normalise,

	kOsc1Coarse,
	kOsc1Fine,

	kNumParams
};

PathogenSynth::PathogenSynth(IPlugInstanceInfo instanceInfo)
  : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
    mSampleRate(44100.),
    mNumHeldKeys(0),
    mKey(-1),
    mActiveVoices(0)

{
  TRACE;

  mTable = new double[TABLE_SIZE];

  for (int i = 0; i < TABLE_SIZE; i++)
  {
	  mTable[i] = 0.0;
	  for (int j = 1; j < 80; j++)
	  { 
		double mult = 0.5 / double(j);
		mTable[i] += mult * sin(  ( double(j*i) /double(TABLE_SIZE)) * 2. * M_PI);
	  }
     printf("mTable[%i] %f\n", i, mTable[i]);
  }


  //mOsc = new CWTOsc(mTable, TABLE_SIZE);
  mEnv = new CADSREnvL();

  memset(mKeyStatus, 0, 128 * sizeof(bool));

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kAttack)->InitDouble("Amp Attack", ATTACK_DEFAULT, TIME_MIN, TIME_MAX, 0.001);
  GetParam(kDecay)->InitDouble("Amp Decay", DECAY_DEFAULT, TIME_MIN, TIME_MAX, 0.001);
  GetParam(kSustain)->InitDouble("Amp Sustain", 1., 0., 1., 0.001);
  GetParam(kRelease)->InitDouble("Amp Release", RELEASE_DEFAULT, TIME_MIN, TIME_MAX, 0.001);

  GetParam(kOsc1Coarse)->InitInt("Osc1 Coarse Tune", 0, -120, 120);
  GetParam(kOsc1Fine)->InitInt("Osc1 Fine Tune", 0, -100, 100);

  GetParam(kOsc1LoopMode)->InitInt("Osc1 Loop Mode", 0, 0, Oscillator::eLoopModes::NUM_LOOP_MODES);
  GetParam(kOsc1Normalise)->InitInt("Osc1 Normalise", 0, 0, 1);


  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(BG_ID, BG_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  pGraphics->AttachControl(new IKnobMultiControl(this, kAttackX, kAttackY, kAttack, &knob));

  IBitmap knobDecay = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  pGraphics->AttachControl(new IKnobMultiControl(this, kAttackX + 120, kAttackY, kDecay, &knobDecay));

  IBitmap knobSustain = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  pGraphics->AttachControl(new IKnobMultiControl(this, kAttackX + 240, kAttackY, kSustain, &knobSustain));

  IBitmap knobRelease = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  pGraphics->AttachControl(new IKnobMultiControl(this, kAttackX + 360, kAttackY, kRelease, &knobRelease));

  IBitmap knobOsc1Coarse = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  pGraphics->AttachControl(new IKnobMultiControl(this, kOsc1CoarseX, kOsc1CoarseY, kOsc1Coarse, &knobOsc1Coarse));

  IBitmap knobOsc1Fine = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  pGraphics->AttachControl(new IKnobMultiControl(this, kOsc1CoarseX + 120, kOsc1CoarseY, kOsc1Fine, &knobOsc1Fine));


  IText text = IText(14);
  IBitmap regular = pGraphics->LoadIBitmap(WHITE_KEY_ID, WHITE_KEY_FN, 6);
  IBitmap sharp   = pGraphics->LoadIBitmap(BLACK_KEY_ID, BLACK_KEY_FN);

  //                    C#     D#          F#      G#      A#
  int coords[12] = { 0, 7, 12, 20, 24, 36, 43, 48, 56, 60, 69, 72 };
  mKeyboard = new IKeyboardControl(this, kKeybX, kKeybY, 48, 5, &regular, &sharp, coords);

  pGraphics->AttachControl(mKeyboard);

  IBitmap about = pGraphics->LoadIBitmap(ABOUTBOX_ID, ABOUTBOX_FN);
  mAboutBox = new IBitmapOverlayControl(this, 100, 100, &about, IRECT(540, 250, 680, 290));
  pGraphics->AttachControl(mAboutBox);


  IBitmap O1Select = pGraphics->LoadIBitmap(OSC1_SELECT_ID, O1_SELECT_FN);

  IBitmap O1SelectUp = pGraphics->LoadIBitmap(OSC1_SELECTUP_ID, O1_SELECTUP_FN);

  Osc1Button = new IBitmapControl(this, 20, 20, &O1SelectUp);
  Osc1Button->SetValueFromPlug(1.);
  Osc1Button->Hide(false);
  pGraphics->AttachControl(Osc1Button);

  mFileSelector = new IFileSelectorControl(this, IRECT(20, 20, 20 +141, 20+20), kSelectFile, &O1Select, kFileOpen, "Wavs", "*");
  pGraphics->AttachControl(mFileSelector);

  pGraphics->HandleMouseOver(true);
  

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);


  WaveRIFFParser* wp = new WaveRIFFParser();
  wp->importWave("Wavs/32bitfloat.wav");

  Wave[0] = new Wavetable();
  Wave[0]->importWave((void*)wp->getBuf(), wp->getFormat(), wp->getChannelCount(), wp->getFrameCount(), wp->getSampleRate());

  Osc1 = new Oscillator(Wave[0]);


  mOsc = new CWTOsc(Wave[0]->getLeftSamples(), Wave[0]->getFrameCount());

  mOsc1Manager = new OscillatorControlManager(this, pGraphics, 170, 10, kOsc1StartPoint);
  mOsc1Manager->getWavDisp()->setWaveformPoints(Osc1->getWavetable());
  mOsc1Manager->AttachControls(pGraphics);

  AttachGraphics(pGraphics);

//  Filt1 = new Filter();




}

PathogenSynth::~PathogenSynth()
{
  delete mOsc;
  delete mEnv;
  delete [] mTable;




}

int PathogenSynth::FindFreeVoice()
{
  int v;

  for(v = 0; v < MAX_VOICES; v++)
  {
    if(!mVS[v].GetBusy())
      return v;
  }

  int quietestVoice = 0;
  double level = 2.;

  for(v = 0; v < MAX_VOICES; v++)
  {
    double summed = mVS[v].mEnv_ctx.mPrev;

    if (summed < level)
    {
      level = summed;
      quietestVoice = v;
    }

  }

  DBGMSG("stealing voice %i\n", quietestVoice);
  return quietestVoice;
}

void PathogenSynth::NoteOnOff(IMidiMsg* pMsg)
{
  int v;

  int status = pMsg->StatusMsg();
  int velocity = pMsg->Velocity();
  int note = pMsg->NoteNumber();

  if (status == IMidiMsg::kNoteOn && velocity) // Note on
  {
    v = FindFreeVoice(); // or quietest
    mVS[v].mKey = note;
    mVS[v].mOsc_ctx.mPhaseIncr = (1./mSampleRate) * midi2CPS(note);
    mVS[v].mEnv_ctx.mLevel = (double) velocity / 127.;
    mVS[v].mEnv_ctx.mStage = kStageAttack;

    mActiveVoices++;
  }
  else  // Note off
  {
    for (v = 0; v < MAX_VOICES; v++)
    {
      if (mVS[v].mKey == note)
      {
        if (mVS[v].GetBusy())
        {
          mVS[v].mKey = -1;
          mVS[v].mEnv_ctx.mStage = kStageRelease;
          mVS[v].mEnv_ctx.mReleaseLevel = mVS[v].mEnv_ctx.mPrev;

          return;
        }
      }
    }
  }

}

void PathogenSynth::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us
  IKeyboardControl* pKeyboard = (IKeyboardControl*) mKeyboard;

  if (pKeyboard->GetKey() != mKey)
  {
    IMidiMsg msg;

    if (mKey >= 0)
    {
      msg.MakeNoteOffMsg(mKey + 48, 0, 0);
      mMidiQueue.Add(&msg);
    }

    mKey = pKeyboard->GetKey();

    if (mKey >= 0)
    {
      msg.MakeNoteOnMsg(mKey + 48, pKeyboard->GetVelocity(), 0, 0);
      mMidiQueue.Add(&msg);
    }
  }

  if (mActiveVoices > 0 || !mMidiQueue.Empty()) // block not empty
  {
    double* out1 = outputs[0];
    double* out2 = outputs[1];

    CVoiceState* vs;

    for (int s = 0; s < nFrames; ++s)
    {
		*out1 = 0.;
		*out2 = 0.;
		while (!mMidiQueue.Empty())
		{
			IMidiMsg* pMsg = mMidiQueue.Peek();

			if (pMsg->mOffset > s) break;

			int status = pMsg->StatusMsg(); // get the MIDI status byte

			switch (status)
			{
				case IMidiMsg::kNoteOn:
				Osc1->trigger((double)pMsg->NoteNumber(), pMsg->Velocity());
				case IMidiMsg::kNoteOff:
				{
			
				NoteOnOff(pMsg);
				break;
				}
				case IMidiMsg::kPitchWheel:
				{
				//TODO
				break;
				}
			}

			mMidiQueue.Remove();
		}

      for(int v = 0; v < MAX_VOICES; v++) // for each vs
      {
        vs = &mVS[v];

        if (vs->GetBusy())
        {
			*out1 += Osc1->getSample(Oscillator::LEFT_CHANNEL) * mEnv->process(&vs->mEnv_ctx);
			*out2 += Osc1->getSample(Oscillator::RIGHT_CHANNEL) * mEnv->process(&vs->mEnv_ctx);
        }
      }

	  Osc1->updatePhase();
	  mOsc1Manager->getWavDisp()->setCurrentSample(Osc1->getCurrentPhase());

	  out1++;
	  out2++;
    }

    mMidiQueue.Flush(nFrames);
  }
//  else // empty block
//  {
//  }
}

void PathogenSynth::Reset()
{
  TRACE;
  IMutexLock lock(this);

  mSampleRate = GetSampleRate();
  mMidiQueue.Resize(GetBlockSize());
  mEnv->setSampleRate(mSampleRate);
}

void PathogenSynth::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kAttack:
      mEnv->setStageTime(kStageAttack, GetParam(kAttack)->Value());
      break;
    case kDecay:
      mEnv->setStageTime(kStageDecay, GetParam(kDecay)->Value());
      break;
    case kSustain:
      mEnv->setSustainLevel( GetParam(kSustain)->Value() );
      break;
    case kRelease:
      mEnv->setStageTime(kStageRelease, GetParam(kRelease)->Value());
      break;

	case kSelectFile:
	{
		WDL_String string;
		mFileSelector->GetLastSelectedFileForPlug(&string);
		DBGMSG(string.Get());

		WaveRIFFParser wp;

		if (wp.importWave(string.Get()) == WaveRIFFParser::eErrorCodes::SUCCESS)
		{
			Wave[0]->importWave((void*)wp.getBuf(), wp.getFormat(), wp.getChannelCount(), wp.getFrameCount(), wp.getSampleRate());
			Osc1->updateWavetable(Wave[0]);

			mOsc1Manager->getWavDisp()->setWaveformPoints(Osc1->getWavetable());
			mOsc1Manager->getWavDisp()->setLoopPoints(	Osc1->getLoopPoint(Oscillator::START),
														Osc1->getLoopPoint(Oscillator::LOOP),
														Osc1->getLoopPoint(Oscillator::END));


		}
	}
	break;

	case kOsc1StartPoint:
	case kOsc1LoopPoint:
	case kOsc1EndPoint:
	{
		Osc1->updateLoopPoints(
			mOsc1Manager->getWavDisp()->getLoopPoint(IWaveformDisplayInteractive::START_POINT),
			mOsc1Manager->getWavDisp()->getLoopPoint(IWaveformDisplayInteractive::LOOP_POINT),
			mOsc1Manager->getWavDisp()->getLoopPoint(IWaveformDisplayInteractive::END_POINT));

		//If smoothing is enabled
		if ( !mOsc1Manager->getWavDisp()->isSetLoopMode() )
		{
			Osc1->smoothLoopPoints(Oscillator::FALLING);
			mOsc1Manager->getWavDisp()->setLoopPoint(IWaveformDisplayInteractive::START_POINT, Osc1->getLoopPoint(Oscillator::START));
			mOsc1Manager->getWavDisp()->setLoopPoint(IWaveformDisplayInteractive::LOOP_POINT, Osc1->getLoopPoint(Oscillator::LOOP));
			mOsc1Manager->getWavDisp()->setLoopPoint(IWaveformDisplayInteractive::END_POINT, Osc1->getLoopPoint(Oscillator::END));
		}
	}
	break;

	case kOsc1Fine:
	{
		DBGMSG("Osc1 Fine = %i\n", GetParam(kOsc1Fine)->Int());
		Osc1->setFineTune( GetParam(kOsc1Fine)->Int() );
		Osc1->updatePhaseInc();
		break;
	}

	case kOsc1Coarse:
	{
		DBGMSG("Osc1 Coarse = %i\n", GetParam(kOsc1Coarse)->Int());
		Osc1->setCoarseTune(GetParam(kOsc1Coarse)->Int());
		Osc1->updatePhaseInc();
		break;
	}

	case kOsc1LoopMode:
	{
		Osc1->setLoopMode( mOsc1Manager->getLoopModeBox()->GetChosenItemIdx() );
		break;
	}

	case kOsc1Normalise:
	{
		if (GetParam(kOsc1Normalise)->Int())
		{
			Osc1->getWavetable()->Normalise();
		}
		else
		{
			Osc1->getWavetable()->DeNormalise();
		}

		mOsc1Manager->getWavDisp()->Redraw();
		break;
	}

    default:
      break;
  }
}

void PathogenSynth::ProcessMidiMsg(IMidiMsg* pMsg)
{
  int status = pMsg->StatusMsg();
  int velocity = pMsg->Velocity();
  
  switch (status)
  {
    case IMidiMsg::kNoteOn:
    case IMidiMsg::kNoteOff:
      // filter only note messages
      if (status == IMidiMsg::kNoteOn && velocity)
      {
        mKeyStatus[pMsg->NoteNumber()] = true;
        mNumHeldKeys += 1;
      }
      else
      {
        mKeyStatus[pMsg->NoteNumber()] = false;
        mNumHeldKeys -= 1;
      }
      break;
    default:
      return; // if !note message, nothing gets added to the queue
  }
  

  mKeyboard->SetDirty();
  mMidiQueue.Add(pMsg);
}

// Should return non-zero if one or more keys are playing.
int PathogenSynth::GetNumKeys()
{
  IMutexLock lock(this);
  return mNumHeldKeys;
}

// Should return true if the specified key is playing.
bool PathogenSynth::GetKeyStatus(int key)
{
  IMutexLock lock(this);
  return mKeyStatus[key];
}

//Called by the standalone wrapper if someone clicks about
bool PathogenSynth::HostRequestingAboutBox()
{
  IMutexLock lock(this);
  if(GetGUI())
  {
    // get the IBitmapOverlay to show
    mAboutBox->SetValueFromPlug(1.);
    mAboutBox->Hide(false);
  }
  return true;
}
