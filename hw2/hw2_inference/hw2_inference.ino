#include <HW2_-_ESD_inferencing.h>
#include <TFT_eSPI.h>

#define RED_LED A0
#define GREEN_LED A1

TFT_eSPI tft;

static float features[1]; 
static ei::signal_t my_signal;

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  pinMode(WIO_LIGHT, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(WIO_BUZZER, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(WIO_BUZZER, LOW);

  my_signal.total_length = 1; 
  my_signal.get_data = [](size_t offset, size_t length, float *out_buffer) -> int {
    out_buffer[0] = features[0];
    return 0;
  };

  tft.println("Edge Impulse model ready...");
}

void loop() {
  int lightValue = analogRead(WIO_LIGHT);
  features[0] = (float)lightValue;

  ei_impulse_result_t result = { 0 };
  EI_IMPULSE_ERROR res = run_classifier(&my_signal, &result, false);
  if (res != EI_IMPULSE_OK) {
    tft.fillScreen(TFT_RED);
    tft.setCursor(0, 0);
    tft.printf("Model failed, code: %d\n", res);
    delay(1000);
    return;
  }

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
float max_confidence = 0;
int max_index = -1;

for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
  if (result.classification[i].value > max_confidence) {
    max_confidence = result.classification[i].value;
    max_index = i;
  }
}

if (max_index >= 0) {
  const char* detected_label = result.classification[max_index].label;

  tft.printf("Detected: %s (%.2f)\n", detected_label, max_confidence);

  if (strcmp(detected_label, "scissor") == 0) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(WIO_BUZZER, LOW);
  } else if (strcmp(detected_label, "rock") == 0) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(WIO_BUZZER, LOW);
  } else if (strcmp(detected_label, "paper") == 0) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    tone(WIO_BUZZER, 1000, 500);
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(WIO_BUZZER, LOW);
  }
}

  delay(500);
}
