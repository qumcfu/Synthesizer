#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QAudioInput>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <math.h>
#include <chrono>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    audioPlayer(this),
    oscillatorOffset(0),
    oscillatorCount(4){

    ui->setupUi(this);

    activeOscillatorId = 0;

    for (int i = 0; i < 4; i++){
        scenes[i] = new QGraphicsScene(this);
        envelopeColors[i] = 255;
        envelopeSampleCount[i] = 0;
        autoInterval[i] = true;
        additionInterval[i] = 0;
    }
    processedSampleScene = new QGraphicsScene(this);
    faceScene = new QGraphicsScene(this);

    ui->graphicsView_0->setScene(scenes[0]);
    ui->graphicsView_1->setScene(scenes[1]);
    ui->graphicsView_2->setScene(scenes[2]);
    ui->graphicsView_3->setScene(scenes[3]);

    ui->faceView->setScene(faceScene);
    ui->processedSamplesView_0->setScene(processedSampleScene);
    recentSampleCount = 200;
    sampleViewRotated = true;

    QBrush greenBrush(Qt::green);
    QBrush blueBrush(Qt::blue);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(2);

    freqBoxes[0] = ui->freqBox_0;
    freqBoxes[1] = ui->freqBox_1;
    freqBoxes[2] = ui->freqBox_2;
    freqBoxes[3] = ui->freqBox_3;

    oscillatorStatus[0] = ui->oscillatorStatus_0;
    oscillatorStatus[1] = ui->oscillatorStatus_1;
    oscillatorStatus[2] = ui->oscillatorStatus_2;
    oscillatorStatus[3] = ui->oscillatorStatus_3;

    oscillatorRoles[0] = ui->oscillatorRole_0;
    oscillatorRoles[1] = ui->oscillatorRole_1;
    oscillatorRoles[2] = ui->oscillatorRole_2;
    oscillatorRoles[3] = ui->oscillatorRole_3;

    oscillatorTypes[0] = ui->oscillatorType_0;
    oscillatorTypes[1] = ui->oscillatorType_1;
    oscillatorTypes[2] = ui->oscillatorType_2;
    oscillatorTypes[3] = ui->oscillatorType_3;

    additionIntervalGroupBoxes[0] = ui->additionIntervalGroup_0;
    additionIntervalGroupBoxes[1] = ui->additionIntervalGroup_1;
    additionIntervalGroupBoxes[2] = ui->additionIntervalGroup_2;
    additionIntervalGroupBoxes[3] = ui->additionIntervalGroup_3;

    additionIntervalComboBoxes[0] = ui->additionIntervalBox_0;
    additionIntervalComboBoxes[1] = ui->additionIntervalBox_1;
    additionIntervalComboBoxes[2] = ui->additionIntervalBox_2;
    additionIntervalComboBoxes[3] = ui->additionIntervalBox_3;

    volumeSliders[0] = ui->volumeSlider_0;
    volumeSliders[1] = ui->volumeSlider_1;
    volumeSliders[2] = ui->volumeSlider_2;
    volumeSliders[3] = ui->volumeSlider_3;

    additionManagerActive = false;

    initializeAudio();
    setActiveOscillator(0);
    timerId = startTimer(20);
    detectionTimerId = startTimer(50);

    for (int i = 0; i < 19; i++){
        keyDown[i] = false;
    }
    keyShift = 0;

    on_filterSelector_currentIndexChanged(QString ("No filter"));
    ui->Note_Other->setVisible(false);

    debugView = true;
    on_debugButton_pressed();

    organizeKeys();
    showHotkeys(false);
    styleSheetWhite = QString("QPushButton {background-color: white; color: black; border-style: outset; border-width: 1px; border-color: black; border-radius: 4px} QPushButton:pressed {background-color: rgb(192, 192, 192); border-color: rgb(64, 64, 64)}");
    styleSheetBlack = QString("QPushButton {background-color: black; color: white; border-style: outset; border-width: 1px; border-color: white; border-radius: 8px} QPushButton:pressed {background-color: rgb(64, 64, 64); border-color: rgb(128, 128, 128)}");
    styleSheetLightGreen = QString("QPushButton {background-color: rgb(102, 205, 0); color: black; border-style: outset; border-width: 1px; border-color: black; border-radius: 4px}");
    styleSheetDarkGreen = QString("QPushButton {background-color: rgb(69, 139, 0); color: black; border-style: outset; border-width: 1px; border-color: black; border-radius: 8px}");
    styleSheetLightBlue = QString("QPushButton {background-color: rgb(126, 192, 238); color: black; border-style: outset; border-width: 1px; border-color: black; border-radius: 4px}");
    styleSheetLightBlueAlt = QString("QPushButton {background-color: rgb(155, 226, 255); color: black; border-style: outset; border-width: 1px; border-color: black; border-radius: 4px}");
    styleSheetDarkBlue = QString("QPushButton {background-color: rgb(54, 100, 139); color: black; border-style: outset; border-width: 1px; border-color: white; border-radius: 8px}");
    styleSheetDarkBlueAlt = QString("QPushButton {background-color: rgb(79, 148, 205); color: black; border-style: outset; border-width: 1px; border-color: white; border-radius: 8px}");
    styleSheetLightRed = QString("QPushButton {background-color: rgb(200, 0, 64); color: black; border-style: outset; border-width: 1px; border-color: black; border-radius: 4px}");

    eyesOpen = true;
}

MainWindow::~MainWindow()
{
    killTimer(timerId);
    killTimer(detectionTimerId);
    delete ui;
}

void MainWindow::initializeAudio(){
    audioPlayer.setAudioSource(&oscillatorSource);
    audioPlayer.start();

    on_volumeSlider_0_valueChanged(ui->volumeSlider_0->value());
    on_volumeSlider_1_valueChanged(ui->volumeSlider_1->value());
    on_volumeSlider_2_valueChanged(ui->volumeSlider_2->value());
    on_volumeSlider_3_valueChanged(ui->volumeSlider_3->value());

    for (int i = 0; i < oscillatorCount; i++){
        updateOscillatorIcon(i, true);
    }
}

void MainWindow::timerEvent(QTimerEvent *event){
    if (event->timerId() == timerId){
        updateVisuals();
    } else if (event->timerId() == detectionTimerId){
        detectionUpdate();
    }
    event->accept();
}

void MainWindow::updateVisuals(){
    oscillatorOffset++;
    if (oscillatorOffset >= INT_MAX - 1){
        oscillatorOffset = 0;
    }
    updateAllOscillatorIcons();
    drawProcessedSampleView();
    if (oscillatorOffset % 100 == 0){
        bool anyOscillatorOn = false;
        for (int i = 0; i < oscillatorCount; i++){
            if (oscillatorSource.isOscillatorOn(i)){
                anyOscillatorOn = true;
            }
        }
        if (!anyOscillatorOn){
            for (int i = 48; i <= 88; i++){
                keys[i]->setStyleSheet(getMatchingStyleSheet(false, !keys[i]->objectName().contains("is") && !keys[i]->objectName().contains("B"), activeOscillatorId, i));
            }
        }
    }
    ui->activeOODLabel->setText(QString("OODs active: %1").arg(QString::number(oscillatorSource.getActiveOODCount())));
    ui->note_AO->setText(QString("Last AO id: %1").arg(QString::number(oscillatorSource.getNoteId(activeOscillatorId))));
    ui->note_OOD_0->setText(QString("OOD #1 id: %1").arg(QString::number(oscillatorSource.getOODNoteId(0))));
    ui->note_OOD_1->setText(QString("OOD #2 id: %1").arg(QString::number(oscillatorSource.getOODNoteId(1))));
}

void MainWindow::setActiveOscillator(int oscillatorIndex){
    int previousId = activeOscillatorId;
    activeOscillatorId = oscillatorIndex;
    oscillatorSource.setActiveOscillatorId(oscillatorIndex);

    if (oscillatorSource.isPartOfModulation(oscillatorIndex)){
        oscillatorSource.setSynthesisType(OscillatorSource::FreqMod);
        showModulationTools(true);
        showAdditionTools(false);
    } else if (oscillatorSource.isPartOfAddition(oscillatorIndex)){
        oscillatorSource.setSynthesisType(OscillatorSource::Add);
        showModulationTools(false);
        showAdditionTools(true);
    } else {
        oscillatorSource.setSynthesisType(OscillatorSource::Simple);
        showModulationTools(false);
        showAdditionTools(false);
    }

    ui->attackBox_0->setValue(oscillatorSource.getAttackSeconds(oscillatorIndex));
    ui->decayBox_0->setValue(oscillatorSource.getDecaySeconds(oscillatorIndex));
    ui->releaseBox_0->setValue(oscillatorSource.getReleaseSeconds(oscillatorIndex));
    ui->sustainBox_0->setValue(oscillatorSource.getSustain_dB(oscillatorIndex));

    ui->attackSlider_0->setValue((int)(oscillatorSource.getAttackSeconds(oscillatorIndex) * 1000.0));
    ui->decaySlider_0->setValue((int)(oscillatorSource.getDecaySeconds(oscillatorIndex) * 1000.0));
    ui->releaseSlider_0->setValue((int)(oscillatorSource.getReleaseSeconds(oscillatorIndex) * 1000.0));
    ui->sustainSlider_0->setValue((int)(oscillatorSource.getSustain_dB(oscillatorIndex) * 10.0));

    ui->pollutionBox_0->setValue(oscillatorSource.getPollution(oscillatorIndex));
    ui->pollutionSlider_0->setValue((int)(oscillatorSource.getPollution(oscillatorIndex) * 1000.0));

    ui->filterSelector->setCurrentIndex(int(oscillatorSource.getFilterType(oscillatorIndex)));
    ui->filterCutoffBox->setValue(double(oscillatorSource.getFilterCutoff(oscillatorIndex)));

    ui->copyFromBox->clear();
    for (int i = 0; i < oscillatorCount; i++){
        if (i != activeOscillatorId){
            ui->copyFromBox->addItem(QString("Oscillator %1").arg(i));
        }
    }
    ui->copyFromBox->setCurrentIndex((previousId > activeOscillatorId) ? previousId - 1 : previousId);

    envelopeColors[activeOscillatorId] = 63;
}

void MainWindow::updateOscillatorIcon(int oscillatorIndex, bool normalized){

    float iterations = 1.5f;
    int sampleCount = 46 * iterations * 2;
    float* samplesArray = new float[sampleCount];
    oscillatorSource.writeIconSamples (samplesArray, oscillatorIndex, sampleCount, true, 0);
    scenes[oscillatorIndex]->clear ();

    if (envelopeColors[oscillatorIndex] < 255){
        drawEnvelope(oscillatorIndex, envelopeColors[oscillatorIndex]);
        envelopeColors[oscillatorIndex]++;
    }

    int lastSample = (int)(samplesArray[0] * 10);
    int canvasWidth = 70;
    for (int i = 0; i <= canvasWidth; i++){
        int sample = (int)(samplesArray[(int)(i * iterations + 0.5f)] * 10);
        scenes[oscillatorIndex]->addLine(i - (canvasWidth / 2), lastSample, i + 1 - (canvasWidth / 2), sample);
        lastSample = sample;
    }

    if (!normalized){
        oscillatorSource.writeIconSamples (samplesArray, oscillatorIndex, sampleCount, false, oscillatorOffset);
        int lastSample = (int)(samplesArray[0] * 10);

        QPen pen;
        if (oscillatorIndex == activeOscillatorId){
            pen = QPen(Qt::blue);
        } else {
            pen = QPen(Qt::green);
        }

        for (int i = 0; i < 71; i++){
            int sample = (int)(samplesArray[(int)(i * iterations + 0.5f)] * 10);
            scenes[oscillatorIndex]->addLine( QLineF (i - (canvasWidth / 2), lastSample, i + 1 - (canvasWidth / 2), sample), pen);
            lastSample = sample;
        }
    }
}

void MainWindow::updateAllOscillatorIcons(){
    for (int i = 0; i < oscillatorCount; i++){
        if (oscillatorSource.isOscillatorParticipating(i)){
            updateOscillatorIcon(i, false);
            oscillatorStatus[i]->setText(QString("On"));
            oscillatorStatus[i]->setStyleSheet("QLabel {color: green}");
        } else {
            updateOscillatorIcon(i, true);
            oscillatorStatus[i]->setText(QString("Off"));
            oscillatorStatus[i]->setStyleSheet("QLabel {color: red}");
        }
    }
    oscillatorStatus[activeOscillatorId]->setText(QString("Active"));
    oscillatorStatus[activeOscillatorId]->setStyleSheet("QLabel {color: blue}");

}

void MainWindow::on_oscillatorType_0_valueChanged(int oscillatorType){
    oscillatorSource.setSelectedOscillator(0, oscillatorType);
    ui->oscillatorType_0->clearFocus();
}

void MainWindow::on_oscillatorType_1_valueChanged(int oscillatorType){
    oscillatorSource.setSelectedOscillator(1, oscillatorType);
    ui->oscillatorType_1->clearFocus();
}

void MainWindow::on_oscillatorType_2_valueChanged(int oscillatorType){
    oscillatorSource.setSelectedOscillator(2, oscillatorType);
    ui->oscillatorType_2->clearFocus();
}

void MainWindow::on_oscillatorType_3_valueChanged(int oscillatorType){
    oscillatorSource.setSelectedOscillator(3, oscillatorType);
    ui->oscillatorType_3->clearFocus();
}

void MainWindow::on_volumeSlider_0_valueChanged(int value){
    oscillatorSource.setGain(0, value / 10.0f);
    ui->volumeLabel_0->setText(QString::number(value / 10) + " dB");
}

void MainWindow::on_volumeSlider_1_valueChanged(int value){
    oscillatorSource.setGain(1, value / 10.0f);
    ui->volumeLabel_1->setText(QString::number(value / 10) + " dB");
}

void MainWindow::on_volumeSlider_2_valueChanged(int value){
    oscillatorSource.setGain(2, value / 10.0f);
    ui->volumeLabel_2->setText(QString::number(value / 10) + " dB");
}

void MainWindow::on_volumeSlider_3_valueChanged(int value){
    oscillatorSource.setGain(3, value / 10.0f);
    ui->volumeLabel_3->setText(QString::number(value / 10) + " dB");
}

void MainWindow::on_freqBox_0_valueChanged(double freq){
    ui->freqSlider_0->setValue((int)freq);
}

void MainWindow::on_freqBox_1_valueChanged(double freq){
    ui->freqSlider_1->setValue((int)freq);
}

void MainWindow::on_freqBox_2_valueChanged(double freq){
    ui->freqSlider_2->setValue((int)freq);
}

void MainWindow::on_freqBox_3_valueChanged(double freq){
    ui->freqSlider_3->setValue((int)freq);
}

void MainWindow::on_freqSlider_0_valueChanged(int freq){
    oscillatorSource.setFrequency(0, freq);
    ui->freqBox_0->setValue(freq);
}

void MainWindow::on_freqSlider_1_valueChanged(int freq){
    oscillatorSource.setFrequency(1, freq);
    ui->freqBox_1->setValue(freq);
}

void MainWindow::on_freqSlider_2_valueChanged(int freq){
    oscillatorSource.setFrequency(2, freq);
    ui->freqBox_2->setValue(freq);
}

void MainWindow::on_freqSlider_3_valueChanged(int freq){
    oscillatorSource.setFrequency(3, freq);
    ui->freqBox_3->setValue(freq);
}

void MainWindow::on_oscillatorSelector_0_pressed(){
    setActiveOscillator(0);
}

void MainWindow::on_oscillatorSelector_1_pressed(){
    setActiveOscillator(1);
}

void MainWindow::on_oscillatorSelector_2_pressed(){
    setActiveOscillator(2);
}

void MainWindow::on_oscillatorSelector_3_pressed(){
    setActiveOscillator(3);
}

void MainWindow::playNote(int oscillatorIndex, int id){

    if (id != -1){
        oscillatorSource.setNote(oscillatorIndex, id + keyShift);
        if (!oscillatorSource.isOscillatorOn(oscillatorIndex)){
            freqBoxes[oscillatorIndex]->setValue(oscillatorSource.noteIdToFrequency(id + keyShift));
        }
        activateNeededOscillators();
        keys[id]->setStyleSheet(getMatchingStyleSheet(true, !keys[id]->objectName().contains("is") && !keys[id]->objectName().contains("B"), oscillatorIndex, id));
    } else {
        int noteId = oscillatorSource.frequencyToNoteId(freqBoxes[oscillatorIndex]->value());
        if (noteId != -1){
            playNote(oscillatorIndex, noteId);
        } else {
            oscillatorSource.setFrequency(oscillatorIndex, freqBoxes[oscillatorIndex]->value());
            activateNeededOscillators();
        }
    }
}

void MainWindow::playMidiNote(int id){
    playNote(activeOscillatorId, id);
}

void MainWindow::activateNeededOscillators(){
    if (oscillatorSource.getSynthesisType() == OscillatorSource::FreqMod && oscillatorSource.isPartOfModulation(activeOscillatorId)){
        oscillatorSource.activateModulation();
    } else if (oscillatorSource.getSynthesisType() == OscillatorSource::Add && oscillatorSource.isPartOfAddition(activeOscillatorId)){
        int additionIds[10] = {0};
        int additionCount = oscillatorSource.copyAdditionIdsToArray(additionIds);
        for (int i = 0; i < additionCount; i++){
            if (i != activeOscillatorId){
                if (autoInterval[additionIds[i]]){
                    oscillatorSource.setNote(additionIds[i], oscillatorSource.getNoteId(activeOscillatorId) + additionInterval[additionIds[i]]);
                    freqBoxes[i]->setValue(oscillatorSource.getFrequency(additionIds[i]));
                }
                int id = oscillatorSource.getNoteId(additionIds[i]);
                if (id != -1){
                    keys[id - keyShift]->setStyleSheet(getMatchingStyleSheet(true, !keys[id - keyShift]->objectName().contains("is") && !keys[id - keyShift]->objectName().contains("B"), additionIds[i], id));
                }
            }
        }
        oscillatorSource.activateAddition();
    } else {
        oscillatorSource.noteOn(activeOscillatorId);
    }
}

void MainWindow::activateAllOscillators(){
    for (int i = 0; i < oscillatorCount; i++){
        playNote(i, -1);
    }
}

void MainWindow::releaseNote(int oscillatorIndex, int id){
    bool needToRecolor = (id == oscillatorSource.getNoteId(oscillatorIndex) && oscillatorSource.getActiveOODCount() > 0);
    oscillatorSource.noteOff(oscillatorIndex, id + keyShift);
//    updateOscillatorIcon(oscillatorIndex, true);
    if (id != -1){
        keys[id]->setStyleSheet(getMatchingStyleSheet(false, !keys[id]->objectName().contains("is") && !keys[id]->objectName().contains("B"), oscillatorIndex, id));
    }
    if (oscillatorSource.getSynthesisType() == OscillatorSource::Add && oscillatorSource.isPartOfAddition(activeOscillatorId)){
            int additionIds[10] = {0};
            int additionCount = oscillatorSource.copyAdditionIdsToArray(additionIds);
            for (int i = 0; i < additionCount; i++){
                if (i != activeOscillatorId){
                    int noteId = oscillatorSource.getNoteId(additionIds[i]) - keyShift;
                    if (noteId != -1 && (noteId == id + additionInterval[additionIds[i]] || !autoInterval[additionIds[i]])){
                        keys[noteId]->setStyleSheet(getMatchingStyleSheet(false, !keys[noteId]->objectName().contains("is") && !keys[noteId]->objectName().contains("B"), additionIds[i], noteId));
                    }
                }
            }
        }
    if (needToRecolor){
        keys[oscillatorSource.getNoteId(oscillatorIndex)]->setStyleSheet(getMatchingStyleSheet(true, !keys[id]->objectName().contains("is") && !keys[id]->objectName().contains("B"), oscillatorIndex, id, true));
    }
}

void MainWindow::releaseMidiNote(int id){
    releaseNote(activeOscillatorId, id);
}

void MainWindow::releaseAllNotes(){
    for (int i = 0; i < oscillatorCount; i++){
//        releaseNote(i, -1);
        for (int id = 48; id < 89; id++){
            releaseNote(i, id);
            keys[id]->setStyleSheet(getMatchingStyleSheet(false, !keys[id]->objectName().contains("is") && !keys[id]->objectName().contains("B"), activeOscillatorId, id));
        }
    }
}

void MainWindow::on_Note_C0_pressed(){
    playNote(activeOscillatorId, 48);
}
void MainWindow::on_Note_C0_released(){
    releaseNote(activeOscillatorId, 48);
}

void MainWindow::on_Note_Cis0_pressed(){
    playNote(activeOscillatorId, 49);
}
void MainWindow::on_Note_Cis0_released(){
    releaseNote(activeOscillatorId, 49);
}

void MainWindow::on_Note_D0_pressed(){
    playNote(activeOscillatorId, 50);
}
void MainWindow::on_Note_D0_released(){
    releaseNote(activeOscillatorId, 50);
}

void MainWindow::on_Note_Dis0_pressed(){
    playNote(activeOscillatorId, 51);
}

void MainWindow::on_Note_Dis0_released(){
    releaseNote(activeOscillatorId, 51);
}

void MainWindow::on_Note_E0_pressed(){
    playNote(activeOscillatorId, 52);
}
void MainWindow::on_Note_E0_released(){
    releaseNote(activeOscillatorId, 52);
}

void MainWindow::on_Note_F0_pressed(){
    playNote(activeOscillatorId, 53);
}
void MainWindow::on_Note_F0_released(){
    releaseNote(activeOscillatorId, 53);
}

void MainWindow::on_Note_Fis0_pressed(){
    playNote(activeOscillatorId, 54);
}
void MainWindow::on_Note_Fis0_released(){
    releaseNote(activeOscillatorId, 54);
}

void MainWindow::on_Note_G0_pressed(){
    playNote(activeOscillatorId, 55);
}
void MainWindow::on_Note_G0_released(){
    releaseNote(activeOscillatorId, 55);
}

void MainWindow::on_Note_Gis0_pressed(){
    playNote(activeOscillatorId, 56);
}
void MainWindow::on_Note_Gis0_released(){
    releaseNote(activeOscillatorId, 56);
}

void MainWindow::on_Note_A0_pressed(){
    playNote(activeOscillatorId, 57);
}
void MainWindow::on_Note_A0_released(){
    releaseNote(activeOscillatorId, 57);
}

void MainWindow::on_Note_B0_pressed(){
    playNote(activeOscillatorId, 58);
}
void MainWindow::on_Note_B0_released(){
    releaseNote(activeOscillatorId, 58);
}

void MainWindow::on_Note_H0_pressed(){
    playNote(activeOscillatorId, 59);
}
void MainWindow::on_Note_H0_released(){
    releaseNote(activeOscillatorId, 59);
}

void MainWindow::on_Note_C1_pressed(){
    playNote(activeOscillatorId, 60);
}
void MainWindow::on_Note_C1_released(){
    releaseNote(activeOscillatorId, 60);
}

void MainWindow::on_Note_Cis1_pressed(){
    playNote(activeOscillatorId, 61);
}
void MainWindow::on_Note_Cis1_released(){
    releaseNote(activeOscillatorId, 61);
}

void MainWindow::on_Note_D1_pressed(){
    playNote(activeOscillatorId, 62);
}
void MainWindow::on_Note_D1_released(){
    releaseNote(activeOscillatorId, 62);
}

void MainWindow::on_Note_Dis1_pressed(){
    playNote(activeOscillatorId, 63);
}
void MainWindow::on_Note_Dis1_released(){
    releaseNote(activeOscillatorId, 63);
}

void MainWindow::on_Note_E1_pressed(){
    playNote(activeOscillatorId, 64);
}
void MainWindow::on_Note_E1_released(){
    releaseNote(activeOscillatorId, 64);
}

void MainWindow::on_Note_F1_pressed(){
    playNote(activeOscillatorId, 65);
}
void MainWindow::on_Note_F1_released(){
    releaseNote(activeOscillatorId, 65);
}

void MainWindow::on_Note_Fis1_pressed(){
    playNote(activeOscillatorId, 66);
}
void MainWindow::on_Note_Fis1_released(){
    releaseNote(activeOscillatorId, 66);
}

void MainWindow::on_Note_G1_pressed(){
    playNote(activeOscillatorId, 67);
}
void MainWindow::on_Note_G1_released(){
    releaseNote(activeOscillatorId, 67);
}

void MainWindow::on_Note_Gis1_pressed(){
    playNote(activeOscillatorId, 68);
}
void MainWindow::on_Note_Gis1_released(){
    releaseNote(activeOscillatorId, 68);
}

void MainWindow::on_Note_A1_pressed(){
    playNote(activeOscillatorId, 69);
}
void MainWindow::on_Note_A1_released(){
    releaseNote(activeOscillatorId, 69);
}

void MainWindow::on_Note_B1_pressed(){
    playNote(activeOscillatorId, 70);
}
void MainWindow::on_Note_B1_released(){
    releaseNote(activeOscillatorId, 70);
}

void MainWindow::on_Note_H1_pressed(){
    playNote(activeOscillatorId, 71);
}
void MainWindow::on_Note_H1_released(){
    releaseNote(activeOscillatorId, 71);
}

void MainWindow::on_Note_C2_pressed(){
    playNote(activeOscillatorId, 72);
}
void MainWindow::on_Note_C2_released(){
    releaseNote(activeOscillatorId, 72);
}

void MainWindow::on_Note_Cis2_pressed(){
    playNote(activeOscillatorId, 73);
}
void MainWindow::on_Note_Cis2_released(){
    releaseNote(activeOscillatorId, 73);
}

void MainWindow::on_Note_D2_pressed(){
    playNote(activeOscillatorId, 74);
}
void MainWindow::on_Note_D2_released(){
    releaseNote(activeOscillatorId, 74);
}

void MainWindow::on_Note_Dis2_pressed(){
    playNote(activeOscillatorId, 75);
}
void MainWindow::on_Note_Dis2_released(){
    releaseNote(activeOscillatorId, 75);
}

void MainWindow::on_Note_E2_pressed(){
    playNote(activeOscillatorId, 76);
}
void MainWindow::on_Note_E2_released(){
    releaseNote(activeOscillatorId, 76);
}

void MainWindow::on_Note_F2_pressed(){
    playNote(activeOscillatorId, 77);
}
void MainWindow::on_Note_F2_released(){
    releaseNote(activeOscillatorId, 77);
}

void MainWindow::on_Note_Fis2_pressed(){
    playNote(activeOscillatorId, 78);
}
void MainWindow::on_Note_Fis2_released(){
    releaseNote(activeOscillatorId, 78);
}

void MainWindow::on_Note_G2_pressed(){
    playNote(activeOscillatorId, 79);
}
void MainWindow::on_Note_G2_released(){
    releaseNote(activeOscillatorId, 79);
}

void MainWindow::on_Note_Gis2_pressed(){
    playNote(activeOscillatorId, 80);
}
void MainWindow::on_Note_Gis2_released(){
    releaseNote(activeOscillatorId, 80);
}

void MainWindow::on_Note_A2_pressed(){
    playNote(activeOscillatorId, 81);
}
void MainWindow::on_Note_A2_released(){
    releaseNote(activeOscillatorId, 81);
}

void MainWindow::on_Note_B2_pressed(){
    playNote(activeOscillatorId, 82);
}
void MainWindow::on_Note_B2_released(){
    releaseNote(activeOscillatorId, 82);
}

void MainWindow::on_Note_H2_pressed(){
    playNote(activeOscillatorId, 83);
}
void MainWindow::on_Note_H2_released(){
    releaseNote(activeOscillatorId, 83);
}

void MainWindow::on_Note_C3_pressed(){
    playNote(activeOscillatorId, 84);
}
void MainWindow::on_Note_C3_released(){
    releaseNote(activeOscillatorId, 84);
}

void MainWindow::on_Note_Cis3_pressed(){
    playNote(activeOscillatorId, 85);
}
void MainWindow::on_Note_Cis3_released(){
    releaseNote(activeOscillatorId, 85);
}

void MainWindow::on_Note_D3_pressed(){
    playNote(activeOscillatorId, 86);
}
void MainWindow::on_Note_D3_released(){
    releaseNote(activeOscillatorId, 86);
}

void MainWindow::on_Note_Dis3_pressed(){
    playNote(activeOscillatorId, 87);
}
void MainWindow::on_Note_Dis3_released(){
    releaseNote(activeOscillatorId, 87);
}

void MainWindow::on_Note_E3_pressed(){
    playNote(activeOscillatorId, 88);
}
void MainWindow::on_Note_E3_released(){
    releaseNote(activeOscillatorId, 88);
}

void MainWindow::on_modIndexSlider_0_valueChanged(int value){
    oscillatorSource.setModulationIndex((float)value / 1000.0f);
    ui->modIndexBox_0->setValue((double)value / 1000.0);
}

void MainWindow::on_modIndexBox_0_valueChanged(double index){
    ui->modIndexSlider_0->setValue((int)(index * 1000.1f));
}

void MainWindow::on_addMultiplierSlider_0_valueChanged(int value){
    oscillatorSource.setAdditionMultiplier((float)value / 1000.0f);
    ui->addMultiplierBox_0->setValue((double)value / 1000.0);
}

void MainWindow::on_addMultiplierBox_0_valueChanged(double value){
    ui->addMultiplierSlider_0->setValue((int)(value * 1000.1f));
}

void MainWindow::on_oscillatorRole_0_currentTextChanged(const QString &role){
    updateOscillatorRoles(0, role);
}

void MainWindow::on_oscillatorRole_1_currentTextChanged(const QString &role){
    updateOscillatorRoles(1, role);
}

void MainWindow::on_oscillatorRole_2_currentTextChanged(const QString &role){
    updateOscillatorRoles(2, role);
}

void MainWindow::on_oscillatorRole_3_currentTextChanged(const QString &role){
    updateOscillatorRoles(3, role);
}

void MainWindow::updateOscillatorRoles(int oscillatorIndex, const QString &role){
    int addIds[4];  // OSCILLATORCOUNT HARD CODED
    int addCount = 0;
    int carrierId = -1;
    int modulatorId = -1;
    for (int i = 0; i < oscillatorCount; i++){
        if (i != oscillatorIndex){
            if (role == "Carrier" || role == "Modulator"){
                if (oscillatorRoles[i]->currentText() == role){
                    oscillatorRoles[i]->setCurrentIndex(0);
                }
            }
        }
        if (oscillatorRoles[i]->currentText() == "Add"){
            addIds[addCount] = i;
            addCount++;
        } else if (oscillatorRoles[i]->currentText() == "Carrier"){
            carrierId = i;
        } else if (oscillatorRoles[i]->currentText() == "Modulator"){
            modulatorId = i;
        }
    }
    if (carrierId != -1 && modulatorId != -1){
        oscillatorSource.deleteAddition();
        oscillatorSource.addModulation(carrierId, modulatorId);
    } else if (addCount > 1){
        oscillatorSource.deleteModulation();
        oscillatorSource.addAddition(addIds, addCount);
    } else {
        oscillatorSource.deleteAddition();
        oscillatorSource.deleteModulation();
    }
////////////////////////////////////////////// not working properly when oscillatorIndex == 2
    if (additionManagerActive && role != "Add"){
        oscillatorRolesBackup[oscillatorIndex] = oscillatorRoles[oscillatorIndex]->currentIndex();
        ui->additionManagementSlider->setValue(oscillatorIndex - 1);
    }

    setActiveOscillator(activeOscillatorId);
}
//////////////////////////////////////////////
void MainWindow::showModulationTools(bool show){
    if (show){
        int carrierId = activeOscillatorId;
        for (int i = 0; i < oscillatorCount; i++){
            if (oscillatorRoles[i]->currentText().contains(("Carrier"))){
                carrierId = i;
                i = oscillatorCount;
            }
        }
        ui->modulationGroup->move(13 + 125 * carrierId, 140);
        ui->modulationGroup->show();
    } else {
        ui->modulationGroup->hide();
    }
}

void MainWindow::showAdditionTools(bool show){
    if (show){
        ui->additionGainGroup->move(13 + 125 * activeOscillatorId, 140);
        ui->additionGainGroup->show();
        for (int i = 0; i < oscillatorCount; i++){
            if (i != activeOscillatorId && oscillatorRoles[i]->currentText().contains("Add")){
                additionIntervalGroupBoxes[i]->show();
            } else {
                additionIntervalGroupBoxes[i]->hide();
            }
        }
    } else {
        ui->additionGainGroup->hide();
        for (int i = 0; i < oscillatorCount; i++){
            additionIntervalGroupBoxes[i]->hide();
        }
    }
}

void MainWindow::organizeKeys(){

    for (int i = 0; i < 120; i++){
        keys[i] = ui->Note_Other;
    }

    keys[48] = ui->Note_C0; keys[49] = ui->Note_Cis0; keys[50] = ui->Note_D0; keys[51] = ui->Note_Dis0; keys[52] = ui->Note_E0;
    keys[53] = ui->Note_F0; keys[54] = ui->Note_Fis0; keys[55] = ui->Note_G0; keys[56] = ui->Note_Gis0; keys[57] = ui->Note_A0; keys[58] = ui->Note_B0; keys[59] = ui->Note_H0;
    keys[60] = ui->Note_C1; keys[61] = ui->Note_Cis1; keys[62] = ui->Note_D1; keys[63] = ui->Note_Dis1; keys[64] = ui->Note_E1;
    keys[65] = ui->Note_F1; keys[66] = ui->Note_Fis1; keys[67] = ui->Note_G1; keys[68] = ui->Note_Gis1; keys[69] = ui->Note_A1; keys[70] = ui->Note_B1; keys[71] = ui->Note_H1;
    keys[72] = ui->Note_C2; keys[73] = ui->Note_Cis2; keys[74] = ui->Note_D2; keys[75] = ui->Note_Dis2; keys[76] = ui->Note_E2;
    keys[77] = ui->Note_F2; keys[78] = ui->Note_Fis2; keys[79] = ui->Note_G2; keys[80] = ui->Note_Gis2; keys[81] = ui->Note_A2; keys[82] = ui->Note_B2; keys[83] = ui->Note_H2;
    keys[84] = ui->Note_C3; keys[85] = ui->Note_Cis3; keys[86] = ui->Note_D3; keys[87] = ui->Note_Dis3; keys[88] = ui->Note_E3;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if (!oscillatorSource.isOscillatorParticipating(activeOscillatorId) || true){   // always true

        switch (event->key()) {
            case Qt::Key_Less: if (!keyDown[1]) playNote(activeOscillatorId, 59); keyDown[1] = true; break;
            case Qt::Key_Y: if (!keyDown[2]) playNote(activeOscillatorId, 60); keyDown[2] = true; break;
            case Qt::Key_S: if (!keyDown[3]) playNote(activeOscillatorId, 61); keyDown[3] = true; break;
            case Qt::Key_X: if (!keyDown[4]) playNote(activeOscillatorId, 62); keyDown[4] = true; break;
            case Qt::Key_D: if (!keyDown[5]) playNote(activeOscillatorId, 63); keyDown[5] = true; break;
            case Qt::Key_C: if (!keyDown[6]) playNote(activeOscillatorId, 64); keyDown[6] = true; break;
            case Qt::Key_V: if (!keyDown[7]) playNote(activeOscillatorId, 65); keyDown[7] = true; break;
            case Qt::Key_G: if (!keyDown[8]) playNote(activeOscillatorId, 66); keyDown[8] = true; break;
            case Qt::Key_B: if (!keyDown[9]) playNote(activeOscillatorId, 67); keyDown[9] = true; break;
            case Qt::Key_H: if (!keyDown[10]) playNote(activeOscillatorId, 68); keyDown[10] = true; break;
            case Qt::Key_N: if (!keyDown[11]) playNote(activeOscillatorId, 69); keyDown[11] = true; break;
            case Qt::Key_J: if (!keyDown[12]) playNote(activeOscillatorId, 70); keyDown[12] = true; break;
            case Qt::Key_M: if (!keyDown[13]) playNote(activeOscillatorId, 71); keyDown[13] = true; break;
            case Qt::Key_Comma: if (!keyDown[14]) playNote(activeOscillatorId, 72); keyDown[14] = true; break;
            case Qt::Key_L: if (!keyDown[15]) playNote(activeOscillatorId, 73); keyDown[15] = true; break;
            case Qt::Key_Period: if (!keyDown[16]) playNote(activeOscillatorId, 74); keyDown[16] = true; break;
            case Qt::Key_Odiaeresis: if (!keyDown[17]) playNote(activeOscillatorId, 75); keyDown[17] = true; break;
            case Qt::Key_Minus: if (!keyDown[18]) playNote(activeOscillatorId, 76); keyDown[18] = true; break;
            case Qt::Key_Down: ui->chordList->setCurrentRow(ui->chordList->currentRow() + 1 <= 10 ? ui->chordList->currentRow() + 1 : 0); break;
            case Qt::Key_Up: ui->chordList->setCurrentRow(ui->chordList->currentRow() - 1 >= 0 ? ui->chordList->currentRow() - 1 : 10); break;
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event){
    switch (event->key()) {
        case Qt::Key_Less: releaseNote(activeOscillatorId, 59); keyDown[1] = false; break;
        case Qt::Key_Y: releaseNote(activeOscillatorId, 60); keyDown[2] = false; break;
        case Qt::Key_S: releaseNote(activeOscillatorId, 61); keyDown[3] = false; break;
        case Qt::Key_X: releaseNote(activeOscillatorId, 62); keyDown[4] = false; break;
        case Qt::Key_D: releaseNote(activeOscillatorId, 63); keyDown[5] = false; break;
        case Qt::Key_C: releaseNote(activeOscillatorId, 64); keyDown[6] = false; break;
        case Qt::Key_V: releaseNote(activeOscillatorId, 65); keyDown[7] = false; break;
        case Qt::Key_G: releaseNote(activeOscillatorId, 66); keyDown[8] = false; break;
        case Qt::Key_B: releaseNote(activeOscillatorId, 67); keyDown[9] = false; break;
        case Qt::Key_H: releaseNote(activeOscillatorId, 68); keyDown[10] = false; break;
        case Qt::Key_N: releaseNote(activeOscillatorId, 69); keyDown[11] = false; break;
        case Qt::Key_J: releaseNote(activeOscillatorId, 70); keyDown[12] = false; break;
        case Qt::Key_M: releaseNote(activeOscillatorId, 71); keyDown[13] = false; break;
        case Qt::Key_Comma: releaseNote(activeOscillatorId, 72); keyDown[14] = false; break;
        case Qt::Key_L: releaseNote(activeOscillatorId, 73); keyDown[15] = false; break;
        case Qt::Key_Period: releaseNote(activeOscillatorId, 74); keyDown[16] = false; break;
        case Qt::Key_Odiaeresis: releaseNote(activeOscillatorId, 75); keyDown[17] = false; break;
        case Qt::Key_Minus: releaseNote(activeOscillatorId, 76); keyDown[18] = false; break;
    }
    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    QWidget* focus = QApplication::focusWidget();
    if (focus != NULL){
        focus->clearFocus();
    }
    event->accept();
}

QString MainWindow::getMatchingStyleSheet(bool buttonDown, bool whiteKey, int oscillator, int noteId, bool forceBase){
    QString sheet = styleSheetLightRed;
    if (buttonDown){
        if (oscillator == activeOscillatorId){
//            qDebug() << QString("Note IDs: %1").arg(oscillatorSource.getNoteId(oscillator));
            if (oscillatorSource.getNoteId(oscillator) == noteId + keyShift || forceBase){
                if (whiteKey){
                    sheet = styleSheetLightBlue;
                } else {
                    sheet = styleSheetDarkBlue;
                }
            } else {
                if (whiteKey){
                    sheet = styleSheetLightBlueAlt;
                } else {
                    sheet = styleSheetDarkBlueAlt;
                }
            }
        } else {
            if (whiteKey){
                sheet = styleSheetLightGreen;
            } else {
                sheet = styleSheetDarkGreen;
            }
        }
    } else {
        if (whiteKey){
            sheet = styleSheetWhite;
        } else {
            sheet = styleSheetBlack;
        }
    }

    return sheet;
}

void MainWindow::on_pollutionBox_0_valueChanged(double value){
    ui->pollutionSlider_0->setValue((int)(value * 1000.0));
}

void MainWindow::on_pollutionSlider_0_valueChanged(int value){
    oscillatorSource.setPollution(activeOscillatorId, (float)value / 1000.0f);
    ui->pollutionBox_0->setValue((double)value / 1000.0);
}

void MainWindow::on_attackBox_0_valueChanged(double attack){
    oscillatorSource.setAttackSeconds(activeOscillatorId, attack);
    ui->attackSlider_0->setValue((int)(attack * 1000.0));
}

void MainWindow::on_attackSlider_0_valueChanged(int attack){
    ui->attackBox_0->setValue((double)attack / 1000.0);
    updateEnvelopeArray(activeOscillatorId);
    envelopeColors[activeOscillatorId] = 0;
}

void MainWindow::on_decayBox_0_valueChanged(double decay){
    oscillatorSource.setDecaySeconds(activeOscillatorId, decay);
    ui->decaySlider_0->setValue((int)(decay * 1000.0));
}

void MainWindow::on_decaySlider_0_valueChanged(int decay){
    ui->decayBox_0->setValue((double)decay / 1000.0);
    updateEnvelopeArray(activeOscillatorId);
    envelopeColors[activeOscillatorId] = 0;
}

void MainWindow::on_sustainBox_0_valueChanged(double sustain){
    oscillatorSource.setSustain_dB(activeOscillatorId, sustain);
    ui->sustainSlider_0->setValue((int)(sustain * 10.0));
}

void MainWindow::on_sustainSlider_0_valueChanged(int sustain){
    ui->sustainBox_0->setValue((double)sustain / 10.0);
    updateEnvelopeArray(activeOscillatorId);
    envelopeColors[activeOscillatorId] = 0;
}

void MainWindow::on_releaseBox_0_valueChanged(double release){
    oscillatorSource.setReleaseSeconds(activeOscillatorId, release);
    ui->releaseSlider_0->setValue((int)(release * 1000.0));
}

void MainWindow::on_releaseSlider_0_valueChanged(int release){
    ui->releaseBox_0->setValue((double)release / 1000.0);
    updateEnvelopeArray(activeOscillatorId);
    envelopeColors[activeOscillatorId] = 0;
}

void MainWindow::updateEnvelopeArray(int oscillatorIndex){
    envelopeSampleCount[oscillatorIndex] = oscillatorSource.updateEnvelopeArray(oscillatorIndex, envelopeValueArray[oscillatorIndex]);
//    scenes[oscillatorIndex]->clear();
}

void MainWindow::drawEnvelope(int oscillatorIndex, int color){
    QPen pen;
    pen = QPen(QColor(color, color, color));
    for (int i = 0; i < envelopeSampleCount[oscillatorIndex]; i++){
        scenes[oscillatorIndex]->addLine(QLineF(i - (envelopeSampleCount[oscillatorIndex] / 2), 11, i - (envelopeSampleCount[oscillatorIndex] / 2), -envelopeValueArray[oscillatorIndex][i] + 11), pen);
    }
}

void MainWindow::drawProcessedSampleView(){
    oscillatorSource.copyRecentSamplesToArray(recentSamples[activeOscillatorId]);
    processedSampleScene->clear();
    int lastSample = (int)(recentSamples[activeOscillatorId][0] * 20.0f);
    for (int i = 0; i < recentSampleCount; i++){
        int sample = (int)(recentSamples[activeOscillatorId][i] * 20.0f);
        if (!sampleViewRotated){
            processedSampleScene->addLine(i - (recentSampleCount / 2), lastSample, i + 1 - (recentSampleCount / 2), sample);
        } else {
            processedSampleScene->addLine(lastSample, i - (recentSampleCount / 2), sample, i + 1 - (recentSampleCount / 2));
        }
        lastSample = sample;
    }
}

void MainWindow::on_copyFromButton_pressed(){
    int origin = ui->copyFromBox->currentIndex();
    if (origin >= activeOscillatorId){
        origin += 1;
    }
    copyEnvelopeData(activeOscillatorId, origin);
}

void MainWindow::on_copyToAllButton_pressed(){
    for (int i = 0; i < oscillatorCount; i++){
        if (i != activeOscillatorId){
            copyEnvelopeData(i, activeOscillatorId);
        }
    }
}

void MainWindow::copyEnvelopeData(int to, int from){
    oscillatorSource.setAttackSeconds(to, oscillatorSource.getAttackSeconds(from));
    oscillatorSource.setDecaySeconds(to, oscillatorSource.getDecaySeconds(from));
    oscillatorSource.setReleaseSeconds(to, oscillatorSource.getReleaseSeconds(from));
    oscillatorSource.setSustain_dB(to, oscillatorSource.getSustain_dB(from));
    updateEnvelopeArray(to);
    envelopeColors[to] = 63;
    if (ui->includeWaveformCheckbox->isChecked()){
        oscillatorTypes[to]->setValue(oscillatorTypes[from]->value());
    }
}

void MainWindow::on_showEnvelopesButton_pressed(){
    for (int i = 0; i < oscillatorCount; i++){
        envelopeColors[i] = 0;
    }
}

void MainWindow::on_updateVisualsCheckBox_toggled(bool checked){
    if (checked){
        timerId = startTimer(50);
    } else {
        killTimer(timerId);
        for (int i = 0; i < oscillatorCount; i++){
            envelopeColors[i] = 255;
            updateOscillatorIcon(i, true);
        }
    }
}

void MainWindow::on_envelopePresets_0_pressed(){
    int previousActiveOscillator = activeOscillatorId;
    int oscillatorsDone = 0;
    for (int i = 0; i < oscillatorCount; i++){
        envelopeColors[i] = 63;
        if (i != previousActiveOscillator){
            setActiveOscillator(i);
            ui->attackSlider_0->setValue(oscillatorsDone != 1 ? 60 : 60);
            ui->decaySlider_0->setValue(oscillatorsDone != 1 ? 3570 : 80);
            ui->releaseSlider_0->setValue(oscillatorsDone != 1 ? 650 : 3440);
            ui->sustainSlider_0->setValue(oscillatorsDone != 1 ? -65 : -625);
            oscillatorTypes[i]->setValue(oscillatorsDone != 1 ? 1 : 2);
            oscillatorsDone++;
        }
    }
    setActiveOscillator(previousActiveOscillator);
    ui->attackSlider_0->setValue(90);
    ui->decaySlider_0->setValue(890);
    ui->releaseSlider_0->setValue(770);
    ui->sustainSlider_0->setValue(-410);
    oscillatorTypes[previousActiveOscillator]->setValue(3);
}

void MainWindow::on_envelopePresets_1_pressed(){
    int previousActiveOscillator = activeOscillatorId;
    for (int i = 0; i < oscillatorCount; i++){
        envelopeColors[i] = 63;
        if (i != previousActiveOscillator){
            setActiveOscillator(i);
            ui->attackSlider_0->setValue(160);
            ui->decaySlider_0->setValue(3350);
            ui->releaseSlider_0->setValue(40);
            ui->sustainSlider_0->setValue(-322);
            oscillatorTypes[i]->setValue(0);
        }
    }
    setActiveOscillator(previousActiveOscillator);
    ui->attackSlider_0->setValue(130);
    ui->decaySlider_0->setValue(3430);
    ui->releaseSlider_0->setValue(2940);
    ui->sustainSlider_0->setValue(-720);
    oscillatorTypes[previousActiveOscillator]->setValue(3);
}

void MainWindow::on_envelopeRandomizer_pressed(){
    int attackValue = (int)((float)rand() / RAND_MAX * 300);
    if (attackValue > 200){
        attackValue = (int)((float)rand() / RAND_MAX * 500);
    }
    if (attackValue > 300){
        attackValue = (int)((float)rand() / RAND_MAX * 750);
    }
    if (attackValue > 500){
        attackValue = (int)((float)rand() / RAND_MAX * 1000);
    }
    ui->attackSlider_0->setValue(attackValue);
    ui->decaySlider_0->setValue((int)((float)rand() / RAND_MAX * 5000));
    ui->releaseSlider_0->setValue((int)((float)rand() / RAND_MAX * 5000));
    ui->sustainSlider_0->setValue((int)((float)rand() / RAND_MAX * -720));
    oscillatorTypes[activeOscillatorId]->setValue((int)((float)rand() / RAND_MAX * 4));
}

void MainWindow::on_additionIntervalSlider_0_valueChanged(int interval){
    int value = interval > 0 ? interval - 1 : interval;
    int baseId = -1;
    if (oscillatorSource.isOscillatorOn(0)){
        baseId = oscillatorSource.getNoteId(activeOscillatorId) - keyShift;
        releaseNote(0, baseId + additionInterval[0]);
    }
    additionInterval[0] = value;
    if (baseId >= 0){
        playNote(activeOscillatorId, baseId);
    }
    ui->additionIntervalBox_0->setCurrentIndex(interval);
}

void MainWindow::on_additionIntervalSlider_1_valueChanged(int interval){
    int value = interval > 0 ? interval - 1 : interval;
    int baseId = -1;
    if (oscillatorSource.isOscillatorOn(1)){
        baseId = oscillatorSource.getNoteId(activeOscillatorId) - keyShift;
        releaseNote(1, baseId + additionInterval[1]);
    }
    additionInterval[1] = value;
    if (baseId >= 0){
        playNote(activeOscillatorId, baseId);
    }
    ui->additionIntervalBox_1->setCurrentIndex(interval);
}

void MainWindow::on_additionIntervalSlider_2_valueChanged(int interval){
    int value = interval > 0 ? interval - 1 : interval;
    int baseId = -1;
    if (oscillatorSource.isOscillatorOn(2)){
        baseId = oscillatorSource.getNoteId(activeOscillatorId) - keyShift;
        releaseNote(2, baseId + additionInterval[2]);
    }
    additionInterval[2] = value;
    if (baseId >= 0){
        playNote(activeOscillatorId, baseId);
    }
    ui->additionIntervalBox_2->setCurrentIndex(interval);
}

void MainWindow::on_additionIntervalSlider_3_valueChanged(int interval){
    int value = interval > 0 ? interval - 1 : interval;
    int baseId = -1;
    if (oscillatorSource.isOscillatorOn(3)){
        baseId = oscillatorSource.getNoteId(activeOscillatorId) - keyShift;
        releaseNote(3, baseId + additionInterval[3]);
    }
    additionInterval[3] = value;
    if (baseId >= 0){
        playNote(activeOscillatorId, baseId);
    }
    ui->additionIntervalBox_3->setCurrentIndex(interval);
}

void MainWindow::on_additionIntervalBox_0_currentIndexChanged(int interval){
    autoInterval[0] = interval != 0;
    ui->additionIntervalSlider_0->setValue(interval);
}

void MainWindow::on_additionIntervalBox_1_currentIndexChanged(int interval){
    autoInterval[1] = interval != 0;
    ui->additionIntervalSlider_1->setValue(interval);
}

void MainWindow::on_additionIntervalBox_2_currentIndexChanged(int interval){
    autoInterval[2] = interval != 0;
    ui->additionIntervalSlider_2->setValue(interval);
}

void MainWindow::on_additionIntervalBox_3_currentIndexChanged(int interval){
    autoInterval[3] = interval != 0;
//    if (autoInterval[3]){ not necessary?!
        ui->additionIntervalSlider_3->setValue(interval);
//    }
}

void MainWindow::on_additionManagementSlider_valueChanged(int count){
    if (!additionManagerActive){
        for (int i = 0; i < oscillatorCount; i++){
            oscillatorRolesBackup[i] = oscillatorRoles[i]->currentIndex();
        }
    }
    additionManagerActive = count > 0;
    for (int i = 0; i < oscillatorCount; i++){
        if (i < count + 1 && additionManagerActive){
            oscillatorRoles[i]->setCurrentIndex(1);
        } else {
            oscillatorRoles[i]->setCurrentIndex(oscillatorRolesBackup[i]);
            if (oscillatorRoles[i]->currentIndex() != 1 && i != activeOscillatorId){
                releaseNote(i, oscillatorSource.getNoteId(i));
            }
        }
    }
}

void MainWindow::on_chordList_currentRowChanged(int chord){
    int additivesFound = 0;
    int chordId = chord;
    int intervals[11][3] = {{4, 7, 12}, {3, 7, 12}, {8, 4, 12}, {6, 3, 12},
                           {11, 4, 7}, {10, 3, 7}, {10, 4, 8}, {9, 3, 6}, {10, 4, 7},
                           {2, 7, 12}, {5, 7, 12}};
/*    QString selectedText = ui->chordList->currentItem()->text();
    if (selectedText == "Major"){
        chordId = 0;
    } else if (selectedText == "Minor"){
        chordId = 1;
    } else if (selectedText == "Augmented"){
        chordId = 2;
    } else if (selectedText == "Diminished"){
        chordId = 3;
    } else if (selectedText == "Major7"){
        chordId = 4;
    } else if (selectedText == "Minor7"){
        chordId = 5;
    } else if (selectedText == "Augmented7"){
        chordId = 6;
    } else if (selectedText == "Diminished7"){
        chordId = 7;
    } else if (selectedText == "Dominant7"){
        chordId = 8;
    } else if (selectedText == "Sus2"){
        chordId = 9;
    } else if (selectedText == "Sus4"){
        chordId = 10;
    }
*/

    if (chordId >= 0){
        for (int i = 0; i < oscillatorCount; i++){
            if (i != activeOscillatorId && oscillatorRoles[i]->currentText().contains("Add")){
                additionIntervalComboBoxes[i]->setCurrentIndex(intervals[chordId][additivesFound] + 1);
                additivesFound++;
            }
        }
    }
    ui->chordList->clearFocus();
}

void MainWindow::on_chordList_pressed(const QModelIndex &index){
    on_chordList_currentRowChanged(index.row());
}

void MainWindow::showHotkeys(bool show){
    QString hotkeys[18] = {"<", "Y", "S", "X", "D", "C", "V", "G", "B", "H", "N", "J", "M", ",", "L", ".", "Ö", "-"};
    for (int i = 0; i < 4; i++){
        keys[48 + 12 * i]->setFont(QFont(QString("Futura"), 8));
        keys[48 + 12 * i]->setText(QString("\n\n\n\n\n\n\nC%1").arg(keyShift / 12 + 3 + i));
    }
    for (int i = 0; i < 18; i++){
        if (!show){
            hotkeys[i] = "";
        } else {
            keys[i + 59]->setFont(QFont(QString("Futura"), 11));
        }
        if (keys[i + 59]->objectName().contains("is") || keys[i + 59]->objectName().contains("B")){
            keys[i + 59]->setText(QString("\n\n%1").arg(hotkeys[i]));
        } else if (show || (i != 1 && i != 13)){
            keys[i + 59]->setText(QString("\n\n\n\n\n%1").arg(hotkeys[i]));
        }

    }
}

void MainWindow::on_showHotkeysCheckBox_toggled(bool checked){
    showHotkeys(checked);
}

void MainWindow::on_playableOctaveSlider_valueChanged(int octaveId){
    keyShift = octaveId * 12;

    showHotkeys(false);
    if (ui->showHotkeysCheckBox->isChecked()){
        showHotkeys(true);
    }
}

void MainWindow::on_additionGainStepSlider_valueChanged(int value){
    oscillatorSource.setAdditionGainModifierStep(value / 10000.0f);
}

void MainWindow::on_filterSelector_currentIndexChanged(const QString &filterType){
    if (filterType == QString("No filter")){
        oscillatorSource.setFilterType(activeOscillatorId, Filter::None);
        ui->filterCutoffBox->setVisible(false);
        ui->filterCutoffSlider->setVisible(false);
    } else if (filterType == QString("Chebyshev Lowpass")){
        oscillatorSource.setFilterType(activeOscillatorId, Filter::ChebyshevLowpass);
        ui->filterCutoffBox->setValue(oscillatorSource.getFilterCutoff(activeOscillatorId));
        ui->filterCutoffBox->setVisible(true);
        ui->filterCutoffSlider->setVisible(true);
    }
}

void MainWindow::on_filterCutoffSlider_valueChanged(int cutoff){
    ui->filterCutoffBox->setValue(double(cutoff));
}

void MainWindow::on_filterCutoffBox_valueChanged(double cutoff){
    oscillatorSource.setFilterCutoff(activeOscillatorId, float(cutoff));
    ui->filterCutoffSlider->setValue(int(cutoff));
}

void MainWindow::on_debugButton_pressed(){
    debugView = !debugView;
    ui->polyphonyGainLabel->setVisible(debugView);
    ui->additionGainStepSlider->setVisible(debugView);
    ui->activeOODLabel->setVisible(debugView);
    ui->note_AO->setVisible(debugView);
    ui->note_OOD_0->setVisible(debugView);
    ui->note_OOD_1->setVisible(debugView);
}

void MainWindow::setDetection(Detection *d){
    detection = d;
}

void MainWindow::detectionUpdate(){
    if (!detection->hasDetectionError()){
        if (eyesOpen != detection->getEyesOpen()){
            eyesOpen = detection->getEyesOpen();
            if (eyesOpen){
                lastSetEyesOpen = std::chrono::steady_clock::now();
                faceScene->clear();
                for (int i = -1; i <= 1; i += 2){
                    faceScene->addLine(5 * i, -2, 10 * i, -2);
                    faceScene->addLine(5 * i, 2, 10 * i, 2);
                    faceScene->addLine(5 * i, -2, 5 * i, 2);
                    faceScene->addLine(10 * i, -2, 10 * i, 2);

                    faceScene->addLine(7 * i, 0, 8 * i, 0);
                    faceScene->addLine(7 * i, 0, 8 * i, 0);
                    faceScene->addLine(7 * i, 0, 7 * i, 0);
                    faceScene->addLine(8 * i, 0, 8 * i, 0);

                }
                if (detectionMode == QString("Random")){
                    if (std::chrono::duration_cast<std::chrono::milliseconds>(lastSetEyesOpen - lastSetEyesClosed).count() < 750){
                        on_envelopeRandomizer_pressed();
                    }
                } else if (detectionMode == QString("Chords")){
                    ui->chordList->setCurrentRow(0);
                } else if (detectionMode == QString("Volume")){
    //                volumeSliders[activeOscillatorId]->setValue(-32);
                }
            } else {
                lastSetEyesClosed = std::chrono::steady_clock::now();
                faceScene->clear();
                for (int i = -1; i <= 1; i += 2){
                    faceScene->addLine(5 * i, -1, 10 * i, -1);
                    faceScene->addLine(5 * i, 1, 10 * i, 1);
                    faceScene->addLine(5 * i, -1, 5 * i, 1);
                    faceScene->addLine(10 * i, -1, 10 * i, 1);
                }
                if (detectionMode == QString("Chords")){
                    ui->chordList->setCurrentRow(1);
                } else if (detectionMode == QString("Volume")){
    //                volumeSliders[activeOscillatorId]->setValue(-32);
                }
            }
        } else {
            if (eyesOpen){
                if (detectionMode == QString("Volume")){
                    int vol = volumeSliders[activeOscillatorId]->value();
                    if (vol < 0){
                        volumeSliders[activeOscillatorId]->setValue(vol + 5);
                    }
                } else if (detectionMode == QString("Modulate")){
                    float random = rand();
                    freqBoxes[1]->setValue(random / RAND_MAX * 1000);
                    on_modIndexBox_0_valueChanged(random / RAND_MAX * 5);
                }
            } else {
                if (detectionMode == QString("Volume")){
                    int vol = volumeSliders[activeOscillatorId]->value();
                    if (vol > -240){
                        volumeSliders[activeOscillatorId]->setValue(vol - 5);
                    }
                }
            }
        }
    } else {
        killTimer(detectionTimerId);
        QPen pen;
        pen = QPen(Qt::red, 2);
        faceScene->addLine( QLineF (31, -4, -31, 4), pen);
    }
}

void MainWindow::on_detectionModeBox_currentTextChanged(const QString &mode){

    if (detectionMode == QString("Chords")){
        ui->additionManagementSlider->setValue(0);
    } else if (detectionMode == QString("Volume")){
        volumeSliders[activeOscillatorId]->setValue(0);
    } else if (detectionMode == QString("Modulate")){
        ui->oscillatorRole_0->setCurrentIndex(oscillatorRolesBackup[0]);
        ui->oscillatorRole_1->setCurrentIndex(oscillatorRolesBackup[1]);
    }

    if (mode == QString("Off")){

    } else if (mode == QString("Random")){

    } else if (mode == QString("Chords")){
        ui->additionManagementSlider->setValue(3);
    } else if (mode == QString("Volume")){
        volumeSliders[activeOscillatorId]->setValue(-240);
    } else if (mode == QString("Modulate")){
        ui->additionManagementSlider->setValue(0);
        oscillatorRolesBackup[0] = oscillatorRoles[0]->currentIndex();
        oscillatorRolesBackup[1] = oscillatorRoles[1]->currentIndex();
        ui->oscillatorRole_0->setCurrentIndex(2);
        ui->oscillatorRole_1->setCurrentIndex(3);
    }
    detectionMode = mode;
}
