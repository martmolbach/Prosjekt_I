#ifndef KEYPADSETUP_H
#define KEYPADSETUP_H

struct KeyPadSetup
{
    // Allows the entering of passwords if this variable is true
    bool enterPasswordStart = false;

    // Allows comparing of passwordHolder and correct password if true
    bool commitPassword = false;

    // variable to update if the door is locked/unlocked
    bool unlockDoor = false;

    // Variable to hold the password input
    String passwordHolder;

    // Serial print the pressed key
    void printKeyPress(char keyPress)
    {
        if (keyPress)
        {
            Serial.print("Keypad input: ");
            Serial.println(keyPress);
        }
    }
    // Allows input to the passwordHolder variable
    // Changes the enterPasswordStart variable to true and commitPassword to false
    void enterPassword(char keyPress)
    {
        if (keyPress == '*')
        {
            enterPasswordStart = true;
            commitPassword = false;
        }
    }
    // Indicates that the password has been entered and is ready to compare
    // Changes the enterPasswordStart variable to false and commitPassword to true
    void passwordCommit(char keyPress)
    {
        if (keyPress == '#')
        {
            commitPassword = true;
            enterPasswordStart = false;
        }
    }
    // Updates the passwordHolder variable if enterPasswordStart variable is true
    // Makes sure the password is a numerical (ONLY WORKS WITH NUMERICALS)
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

    // Compares passwordHolder variable to the correct password
    // Unlocks the door if true alerts the owner of the door if password is wrong
    // commitPassword MUST be true to run
    void passwordCompare()
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
};

#endif
