#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QGroupBox>

#include <QFile>
#include <QAudioOutput>
#include "audioplayer.h"
#include "oscillatorsource.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void playMidiNote (int id);
    void releaseMidiNote(int id);

private slots:
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void setActiveOscillator(int oscillatorIndex);

    void updateOscillatorIcon(int oscillatorIndex, bool normalized);
    void updateAllOscillatorIcons();
    void updateVisuals();
    void showHotkeys(bool show);

    void on_oscillatorType_0_valueChanged(int oscillatorType);
    void on_oscillatorType_1_valueChanged(int oscillatorType);
    void on_oscillatorType_2_valueChanged(int oscillatorType);
    void on_oscillatorType_3_valueChanged(int oscillatorType);
    void playNote (int oscillatorIndex, int id);
    void activateNeededOscillators();
    void activateAllOscillators();
    void releaseNote (int oscillatorIndex, int id);
    void releaseAllNotes();

    void on_volumeSlider_0_valueChanged(int value);
    void on_volumeSlider_1_valueChanged(int value);
    void on_volumeSlider_2_valueChanged(int value);
    void on_volumeSlider_3_valueChanged(int value);

    void on_Note_C0_pressed();      void on_Note_C0_released();
    void on_Note_Cis0_pressed();    void on_Note_Cis0_released();
    void on_Note_D0_pressed();      void on_Note_D0_released();
    void on_Note_Dis0_pressed();    void on_Note_Dis0_released();
    void on_Note_E0_pressed();      void on_Note_E0_released();
    void on_Note_F0_pressed();      void on_Note_F0_released();
    void on_Note_Fis0_pressed();    void on_Note_Fis0_released();
    void on_Note_G0_pressed();      void on_Note_G0_released();
    void on_Note_Gis0_pressed();    void on_Note_Gis0_released();
    void on_Note_A0_pressed();      void on_Note_A0_released();
    void on_Note_B0_pressed();      void on_Note_B0_released();
    void on_Note_H0_pressed();      void on_Note_H0_released();

    void on_Note_C1_pressed();       void on_Note_C1_released();
    void on_Note_Cis1_pressed();    void on_Note_Cis1_released();
    void on_Note_D1_pressed();       void on_Note_D1_released();
    void on_Note_Dis1_pressed();    void on_Note_Dis1_released();
    void on_Note_E1_pressed();       void on_Note_E1_released();
    void on_Note_F1_pressed();       void on_Note_F1_released();
    void on_Note_Fis1_pressed();    void on_Note_Fis1_released();
    void on_Note_G1_pressed();       void on_Note_G1_released();
    void on_Note_Gis1_pressed();    void on_Note_Gis1_released();
    void on_Note_A1_pressed();       void on_Note_A1_released();
    void on_Note_B1_pressed();      void on_Note_B1_released();
    void on_Note_H1_pressed();       void on_Note_H1_released();

    void on_Note_C2_pressed();      void on_Note_C2_released();
    void on_Note_Cis2_pressed();    void on_Note_Cis2_released();
    void on_Note_D2_pressed();      void on_Note_D2_released();
    void on_Note_Dis2_pressed();    void on_Note_Dis2_released();
    void on_Note_E2_pressed();      void on_Note_E2_released();
    void on_Note_F2_pressed();      void on_Note_F2_released();
    void on_Note_Fis2_pressed();    void on_Note_Fis2_released();
    void on_Note_G2_pressed();      void on_Note_G2_released();
    void on_Note_Gis2_pressed();    void on_Note_Gis2_released();
    void on_Note_A2_pressed();      void on_Note_A2_released();
    void on_Note_B2_pressed();      void on_Note_B2_released();
    void on_Note_H2_pressed();      void on_Note_H2_released();

    void on_Note_C3_pressed();      void on_Note_C3_released();
    void on_Note_Cis3_pressed();    void on_Note_Cis3_released();
    void on_Note_D3_pressed();      void on_Note_D3_released();
    void on_Note_Dis3_pressed();    void on_Note_Dis3_released();
    void on_Note_E3_pressed();      void on_Note_E3_released();

    void on_freqBox_0_valueChanged(double freq);
    void on_freqBox_1_valueChanged(double freq);
    void on_freqBox_2_valueChanged(double freq);
    void on_freqBox_3_valueChanged(double freq);
    void on_freqSlider_0_valueChanged(int freq);
    void on_freqSlider_1_valueChanged(int freq);
    void on_freqSlider_2_valueChanged(int freq);
    void on_freqSlider_3_valueChanged(int freq);

    void on_oscillatorSelector_0_pressed();
    void on_oscillatorSelector_1_pressed();
    void on_oscillatorSelector_2_pressed();
    void on_oscillatorSelector_3_pressed();

    void on_modIndexSlider_0_valueChanged(int value);

    void on_modIndexBox_0_valueChanged(double index);

    void on_addMultiplierSlider_0_valueChanged(int value);
    void on_addMultiplierBox_0_valueChanged(double value);

    void on_oscillatorRole_0_currentTextChanged(const QString &role);
    void on_oscillatorRole_1_currentTextChanged(const QString &role);
    void on_oscillatorRole_2_currentTextChanged(const QString &role);
    void on_oscillatorRole_3_currentTextChanged(const QString &role);
    void updateOscillatorRoles(int oscillatorIndex, const QString &role);

    void showModulationTools(bool show);
    void showAdditionTools(bool show);

    void on_pollutionBox_0_valueChanged(double value);
    void on_pollutionSlider_0_valueChanged(int value);

    void on_attackBox_0_valueChanged(double attack);
    void on_attackSlider_0_valueChanged(int attack);
    void on_decayBox_0_valueChanged(double decay);
    void on_decaySlider_0_valueChanged(int decay);
    void on_releaseBox_0_valueChanged(double release);
    void on_releaseSlider_0_valueChanged(int release);
    void on_sustainBox_0_valueChanged(double sustain);
    void on_sustainSlider_0_valueChanged(int sustain);

    void updateEnvelopeArray(int oscillatorIndex);
    void drawEnvelope(int oscillatorIndex, int color);

    void drawProcessedSampleView();

    void on_copyFromButton_pressed();
    void on_copyToAllButton_pressed();
    void copyEnvelopeData(int to, int from);

    void on_showEnvelopesButton_pressed();

    void on_updateVisualsCheckBox_toggled(bool checked);
    QString getMatchingStyleSheet(bool buttonDown, bool whiteKey, int oscillator, int noteId, bool forceBase = false);
    void organizeKeys();

    void on_envelopePresets_0_pressed();

    void on_envelopeRandomizer_pressed();

    void on_envelopePresets_1_pressed();

    void on_additionIntervalSlider_0_valueChanged(int interval);
    void on_additionIntervalSlider_1_valueChanged(int interval);
    void on_additionIntervalSlider_2_valueChanged(int interval);
    void on_additionIntervalSlider_3_valueChanged(int interval);

    void on_additionIntervalBox_0_currentIndexChanged(int interval);
    void on_additionIntervalBox_1_currentIndexChanged(int interval);
    void on_additionIntervalBox_2_currentIndexChanged(int interval);
    void on_additionIntervalBox_3_currentIndexChanged(int interval);

    void on_additionManagementSlider_valueChanged(int count);
    void on_chordList_currentRowChanged(int chord);
    void on_chordList_pressed(const QModelIndex &index);

    void on_showHotkeysCheckBox_toggled(bool checked);

    void on_playableOctaveSlider_valueChanged(int octaveId);

    void on_additionGainStepSlider_valueChanged(int value);

    void on_filterSelector_currentIndexChanged(const QString &filterType);
    void on_filterCutoffSlider_valueChanged(int cutoff);
    void on_filterCutoffBox_valueChanged(double cutoff);

    void on_debugButton_pressed();

private:
    void initializeAudio();
    Ui::MainWindow *ui;
    QGraphicsScene* scenes[4];
    QGraphicsScene* processedSampleScene;
    QGraphicsScene* debugScene;
    OscillatorSource oscillatorSource;
    AudioPlayer audioPlayer;
    int timerId;
    int oscillatorOffset;
    int activeOscillatorId;
    int oscillatorCount;
    int envelopeValueArray[4][70];
    int envelopeColors[4];
    int envelopeSampleCount[4];
    QDoubleSpinBox* freqBoxes[4];
    QLabel* oscillatorStatus[4];
    QComboBox* oscillatorRoles[4];
    QSpinBox* oscillatorTypes[4];
    QGroupBox* additionIntervalGroupBoxes[4];
    QComboBox* additionIntervalComboBoxes[4];
    int recentSampleCount;
    float recentSamples[4][200];
    bool keyDown[19];
    QPushButton* keys[120];
    QString styleSheetWhite, styleSheetBlack, styleSheetLightGreen, styleSheetDarkGreen, styleSheetLightBlue, styleSheetLightBlueAlt, styleSheetDarkBlue, styleSheetDarkBlueAlt, styleSheetLightRed;
    bool autoInterval[4];
    int additionInterval[4];
    int oscillatorRolesBackup[4];
    bool additionManagerActive;
    int keyShift;
    bool sampleViewRotated;
    bool debugView;

};

#endif // MAINWINDOW_H
