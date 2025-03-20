#ifndef CLI_TYPES_H_
#define CLI_TYPES_H_

enum AngleMode {
    ANGLE_MODE_DEG,
    ANGLE_MODE_RAD,
};

enum OutputMode {
    OUTPUT_MODE_NORMAL,
    OUTPUT_MODE_SCIENTIFIC,
    OUTPUT_MODE_ENGINEERING,
};

struct MC4_Settings {
    enum AngleMode angle_mode;
    enum OutputMode output_mode;
};

enum SetttingName {
    SETNAME_UNKOWN,
    SETNAME_ANGLE_MODE,
};

static struct MC4_Settings settings_default() {
    return (struct MC4_Settings){
        .angle_mode = ANGLE_MODE_RAD,
        .output_mode = OUTPUT_MODE_NORMAL,
    };
}

#endif
