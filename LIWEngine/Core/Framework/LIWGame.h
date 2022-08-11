#pragma once
#include "LIWConfig.h"

#include <iostream>
#include <vector>

#include "LIWObject.h"
#include "Memory/LIWMemory.h"
#include "Application/Environment.h"
#include "Debug/LIWDebug.h"
#include "LIWEntityManager.h"
#include "LIWECSConfiguration.h"
#include "LIWFrame.h"

//#include "TestRenderer.h"


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
            liw_delete_static(m_debug);
        }

        void InitGame(LIWPointer<App::Environment, LIWMem_Static> environment) {
            m_currentEnvironment = environment;
            m_debug = liw_new_static<LIWDebug>(environment->m_window->GetTextOutput());
        }

    public:
        LIWPointer<LIWDebug, LIWMem_Static> m_debug{ liw_c_nullhdl };

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
        virtual int Update(LIWPointer<LIWFrameData, LIWMem_Frame> frameData, LIWFiberWorkerPointer thisFiber);
        /*
        * Func: CleanUp
        * Description:  Cleanup Game before exiting program.
        * *             Note: Don't need to override this one usually.
        */
        virtual int CleanUp();

    //protected:
    public:
        LIWPointer<App::Environment, LIWMem_Static> m_currentEnvironment{ liw_c_nullhdl };

        int m_realHZ = m_idealHZ;
        float m_realDT = m_idealDT;
        float m_dtFixedAccum = 0.0f;

    public: 
        LIW::LIW__ComponentCollectionType m__componentCollection;
        LIW::LIWEntityManager m_entityManager{};
    };
}


class LIW_FT_GameUpdate final :
    public LIWFiberTask
{
public:
    void Execute(LIWFiberWorkerPointer thisFiber) override;
public:
    LIWPointer<LIWFrameData, LIWMem_Frame> m_ptrFrameData{};
};