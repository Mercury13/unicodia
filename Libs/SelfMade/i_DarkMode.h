#pragma once

class QString;

namespace dark {

    extern QString fileName;

    enum class Mode : unsigned char {
        AUTO,
        ALWAYS_LIGHT,
        ALWAYS_DARK
    };

    /// @return [+] whether dark is on according to OS settings
    bool doesSystemWant();

    /// @return [+] whether dark is on according to OS settings and mode
    bool doesUserWant(Mode mode);

    /// @return [+] dark mode [-] light mode
    bool isActuallyOn();

    /// Forcefully sets application to dark mode
    void forceOn();

    /// Forcefully sets application to light mode
    void forceOff();

    /// Sets application to dark mode unless it’s actually dark
    void turnOn();

    /// Sets application to light mode unless it’s actually light
    void turnOff();

    /// Sets application to dark/light mode depending on x and whether we already set
    void set(bool x);

    /// Sets application to dark/light mode depending on mode etc
    void set(Mode mode);
}
