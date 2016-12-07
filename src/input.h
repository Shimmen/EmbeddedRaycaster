#ifndef INPUT_H_
#define INPUT_H_

typedef struct input_data {
    int left, right, forward, backward;
    int x_axis, y_axis;
    int should_exit;
} InputData;

void input_init(InputData *input_data);
void get_input(InputData *input_data);

#endif // INPUT_H_