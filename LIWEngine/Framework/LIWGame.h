#pragma once
#include <iostream>
#include <vector>

#include "LIWObject.h"

#include "..\Environment.h"
#include "LIWSystem.h"
#include "LIWStandardSystem.h"
#include "LIWGameTimer.h"
#include "LIWDebug.h"


namespace LIW {
    /*
    * Class: LIWGame
    * Author: tigerccx
    * Description: A class representing the current game. 
    */
    class LIWGame :
        public LIWObject
    {
    public:
        LIWGame(App::Environment* environment) :
            m_currentEnvironment(environment) {
            m_debug = new LIWDebug(environment->m_window->GetTextOutput());
            m_timer = new LIWGameTimer();
        }
        virtual ~LIWGame() {
            delete m_timer;
            delete m_debug;
        }
    public:
        static LIWGame* instance;

        LIWGameTimer* m_timer;
        LIWDebug* m_debug;

        int m_idealHZ = 120;
        float m_idealDT = 1.0f / m_idealHZ;

        /*
        * Func: Initialise
        * Description:  Initialise game which includes:
        *               * Create, initialise all Assets
        *               * Create, initialise and add Systems
        */
        virtual int Initialise();
        /*
        * Func: UpdateMainLoop
        * Description:  FrameUpdate the Game part of main loop.
        *               Note: Don't need to override this one usually.
        */
        virtual int UpdateMainLoop();
        /*
        * Func: CleanUp
        * Description:  Cleanup Game before exiting program.
        * *             Note: Don't need to override this one usually.
        */
        virtual int CleanUp();

    protected:
        std::vector<LIWStandardSystem*> m_standardSystems;

        App::Environment* m_currentEnvironment;

        int m_realHZ = m_idealHZ;
        float m_realDT = m_idealDT;
        float m_dtFixedAccum = 0.0f;
    };
}


