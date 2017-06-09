int const speaker_list[] = { A0, A1, A2, A3, A4, A5 };
int const num_speakers = sizeof(speaker_list) / sizeof(int);

unsigned int speaker_histogram[num_speakers];
unsigned long noise_start_time = 0;
bool in_noise = false;
unsigned long const noise_duration = 100;
int const noise_threshold = 800;

void setup() {
  memset(speaker_histogram, 0, sizeof(speaker_histogram));

  Serial.begin(9600);
}

// Returns the index with the maximum value, and returns the value via a return argument
int max_idx_and_val(unsigned int * vals, int len, int * ret_val) {
  if (len <= 0) {
    *ret_val = 0;
    return 0;
  }

  int max_v = vals[0];
  int max_i = 0;
  for (int i = 1; i < len; i++) { // loop intentionally starts at 1
    if (vals[i] > max_v) {
      max_v = vals[i];
      max_i = i;
    }
  }

  *ret_val = max_v;
  return max_i;
}

void loop() {
  unsigned int speaker_values[num_speakers];
  unsigned long cur_time = millis();

  for (int i = 0; i < num_speakers; i++) {
    speaker_values[i] = analogRead(speaker_list[i]);

    if (speaker_values[i] > noise_threshold) {
      speaker_histogram[i] += 1;

      if (!in_noise) {
        in_noise = true;
        noise_start_time = cur_time;
      }
    }
  }

  if (in_noise && cur_time - noise_start_time > noise_duration) {
    // End noise and report the most commonly activated microphone
    int max_noise_count = 0; // Represents the count of times that a speaker reported a volume higher than the threshold
    int max_noise_idx = max_idx_and_val(speaker_histogram, num_speakers, & max_noise_count);

    // reset the histogram
    memset(speaker_histogram, 0, sizeof(speaker_histogram));
    in_noise = false; // not in a noise anymore

    // Be sure that the noises are "significant"
    if (max_noise_count >= 3) {
      Serial.write((uint8_t) max_noise_idx); // report the speaker that got most of the noise
  //    Serial.println((uint8_t) max_noise_idx);
    }
  }
}

