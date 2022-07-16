#pragma once
#include <iostream>
#include <vector>

#include "LIWObject.h"

#include "Application/Environment.h"
#include "Debug/LIWDebug.h"

#include "TestRenderer.h"


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
        LIWGame(){}
        virtual ~LIWGame() {
            delete m_debug;
        }

        void InitGame(App::Environment* environment) {
            m_currentEnvironment = environment;
            m_debug = new LIWDebug(environment->m_window->GetTextOutput());
        }

    public:
        static LIWGame* instance;

        LIWDebug* m_debug{ nullptr };

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

    //protected:
    public:
        App::Environment* m_currentEnvironment{ nullptr };

        int m_realHZ = m_idealHZ;
        float m_realDT = m_idealDT;
        float m_dtFixedAccum = 0.0f;

        TestRenderer* m_renderer{ nullptr };
    };
}


#include "Memory/LIWMemory.h"
#include "Threading/LIWFiber.h"

struct GameData {
    LIWPointer<LIWFrameData, LIWMem_Frame> m_hdlFrameData;
    LIW::LIWGame* m_game;
};