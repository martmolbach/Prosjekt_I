#ifndef KEYPADSETUP_H
#define KEYPADSETUP_H

struct KeyPadSetup
{
    bool enterPasswordStart = false;
    bool commitPassword = false;
    bool unlockDoor = false;
    String passwordHolder;

    void printKeyPress(char keyPress)
    {
        if (keyPress)
        {
            Serial.print("Keypad input: ");
            Serial.println(keyPress);
        }
    }

    void enterPassword(char keyPress)
    {
        if (keyPress == '*')
        {
            enterPasswordStart = true;
            commitPassword = false;
        }
    }

    void passwordCommit(char keyPress)
    {
        if (keyPress == '#')
        {
            commitPassword = true;
            enterPasswordStart = false;
        }
    }

    void storePasswordInput(char keyPress)
    {
        if (enterPasswordStart == true)
        {
            if (isalnum(keyPress))
            {
                passwordHolder += keyPress;
            }
        }
    }
    void passwordCommit()
    {
        if (commitPassword == true)
        {
            if (passwordHolder == "3265")
            {
                unlockDoor = true;
                Serial.print("Door has been opened");
                Serial.println();
            }
            else
            {
                Serial.print("Wrong password alert");
                Serial.println();
            }
            passwordHolder = '\0';
            commitPassword = false;
        }
    }

    void keyPadSerialPrint()
    {
        Serial.print("Keypad Inputs: ");
        Serial.println(passwordHolder);
    }
};

#endif
