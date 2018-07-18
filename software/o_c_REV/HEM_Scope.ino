class Scope : public HemisphereApplet {
public:

    const char* applet_name() {
        return "Scope";
    }

    void Start() {
        last_tick = OC::CORE::ticks;
        bpm = 0;
        sample_ticks = 320;
        freeze = 0;
    }

    void Controller() {
        if (Clock(0)) {
            int this_tick = OC::CORE::ticks;
            int time = this_tick - last_tick;
            last_tick = this_tick;
            bpm = 1000000 / time;
            if (bpm > 9999) bpm = 9999;
        }

        if (!freeze) {
            last_cv = In(1);

            if (--sample_countdown < 1) {
                sample_countdown = sample_ticks;
                if (++sample_num > 63) sample_num = 0;
                int sample = Proportion(In(0), HEMISPHERE_MAX_CV, 128);
                sample = constrain(sample, -128, 127) + 127;
                snapshot[sample_num] = (uint8_t)sample;
            }

            ForEachChannel(ch) Out(ch, In(ch));
        }
    }

    void View() {
        gfxHeader(applet_name());
        DrawBPM();
        DrawInput1();
        DrawInput2();
        if (freeze) {
            gfxInvert(0, 24, 64, 40);
        }
    }

    void ScreensaverView() {
        DrawBPM();
        DrawInput1();
        DrawInput2();
    }

    void OnButtonPress() {
        freeze = 1 - freeze;
    }

    void OnEncoderMove(int direction) {
        sample_ticks = constrain(sample_ticks += (direction * 10), 64, 64000);
        last_encoder_move = OC::CORE::ticks;
    }
        
    uint32_t OnDataRequest() {
        uint32_t data = 0;
        // example: pack property_name at bit 0, with size of 8 bits
        // Pack(data, PackLocation {0,8}, property_name); 
        return data;
    }

    void OnDataReceive(uint32_t data) {
        // example: unpack value at bit 0 with size of 8 bits to property_name
        // property_name = Unpack(data, PackLocation {0,8}); 
    }

protected:
    void SetHelp() {
        //                               "------------------" <-- Size Guide
        help[HEMISPHERE_HELP_DIGITALS] = "1=Clock (BPM)";
        help[HEMISPHERE_HELP_CVS]      = "1=CV1 2=CV2";
        help[HEMISPHERE_HELP_OUTS]     = "A=CV1 B=CV2";
        help[HEMISPHERE_HELP_ENCODER]  = "T=SmplRate P=Freez";
        //                               "------------------" <-- Size Guide
    }
    
private:
    // BPM Calcultion
    int last_tick;
    int bpm;

    // CV monitor
    int last_cv;
    bool freeze;

    // Scope
    uint8_t snapshot[64];
    int sample_ticks; // Ticks between samples
    int sample_countdown; // Last time a sample was taken
    int sample_num; // Current sample number at the start
    int last_encoder_move; // The last the the sample_ticks value was changed

    // Icons
    const uint8_t clock[8] = {0x9c, 0xa2, 0xc1, 0xcf, 0xc9, 0xa2, 0x9c, 0x00};
    const uint8_t cv[8] = {0x1f, 0x11, 0x11, 0x00, 0x07, 0x18, 0x07, 0x00};

    void DrawBPM() {
        gfxPrint(9, 15, "BPM ");
        gfxPrint(bpm);
        gfxLine(0, 24, 63, 24);

        if (OC::CORE::ticks - last_tick < 1666) gfxBitmap(1, 15, 8, clock);
    }

    void DrawInput1() {
        for (int s = 0; s < 64; s++)
        {
            int x = s + sample_num;
            if (x > 63) x -= 64;
            int l = Proportion(snapshot[x], 255, 28);
            gfxPixel(x, (28 - l) + 24);
        }

        if (OC::CORE::ticks - last_encoder_move < 16667) {
            gfxPrint(1, 26, sample_ticks);
        }
    }

    void DrawInput2() {
        gfxLine(0, 53, 63, 53);
        gfxBitmap(1, 55, 8, cv);
        gfxPrint(10, 55, last_cv);
    }
};


////////////////////////////////////////////////////////////////////////////////
//// Hemisphere Applet Functions
///
///  Once you run the find-and-replace to make these refer to Scope,
///  it's usually not necessary to do anything with these functions. You
///  should prefer to handle things in the HemisphereApplet child class
///  above.
////////////////////////////////////////////////////////////////////////////////
Scope Scope_instance[2];

void Scope_Start(int hemisphere) {
    Scope_instance[hemisphere].BaseStart(hemisphere);
}

void Scope_Controller(int hemisphere, bool forwarding) {
    Scope_instance[hemisphere].BaseController(forwarding);
}

void Scope_View(int hemisphere) {
    Scope_instance[hemisphere].BaseView();
}

void Scope_Screensaver(int hemisphere) {
    Scope_instance[hemisphere].BaseScreensaverView();
}

void Scope_OnButtonPress(int hemisphere) {
    Scope_instance[hemisphere].OnButtonPress();
}

void Scope_OnEncoderMove(int hemisphere, int direction) {
    Scope_instance[hemisphere].OnEncoderMove(direction);
}

void Scope_ToggleHelpScreen(int hemisphere) {
    Scope_instance[hemisphere].HelpScreen();
}

uint32_t Scope_OnDataRequest(int hemisphere) {
    return Scope_instance[hemisphere].OnDataRequest();
}

void Scope_OnDataReceive(int hemisphere, uint32_t data) {
    Scope_instance[hemisphere].OnDataReceive(data);
}