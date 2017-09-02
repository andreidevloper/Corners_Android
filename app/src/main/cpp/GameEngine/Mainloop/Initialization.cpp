#include "GameEngineStd.h"

// Emulate Log2F Function
double Log_2( double n )
{
    // log(n)/log(2) is log2.
    return log(n) / log(2);
}

float Log_2f (float x)
{
    return (float)Log_2((double)x);
}

BaseGameOptions::BaseGameOptions()
{
    m_SimulationOptionFileName = nullptr;
    m_bIsTouchEnabled = false;
}

BaseGameOptions::~BaseGameOptions()
{

}

bool BaseGameOptions::InitBaseGameOptions(String xmlFilePath)
{
    // Read the XML file
    m_SimulationOptionFileName = String(xmlFilePath);

    ResourceCache* res = g_pApp->GetResCache();
    m_pSimulationOptionFile = res->GetResource<XMLFile>(
            m_SimulationOptionFileName); // new Urho3D::XMLFile(g_pApp->GetContext());
    
    if(!m_pSimulationOptionFile)
    {
        URHO3D_LOGERROR("Failed to get XML root from: " + m_SimulationOptionFileName);
        return false;
    }

    XMLElement root = m_pSimulationOptionFile->GetRoot();
    if (!root)
    {
        URHO3D_LOGERROR("Failed to get XML root from: " + m_SimulationOptionFileName);
        return false;
    }

    // Loop through each child element and load the component
    XMLElement node;
    node = root;

    String attribute = node.GetAttribute("name");
    if (node.GetAttribute("name").Compare("options") == 0)
    {
        XMLElement childNode = node.GetChild("UserInterface");
        if (childNode)
        {
            attribute = childNode.GetAttribute("legacyMode");
            m_DebugOptions.m_bIsLegacyMode = (attribute == "yes") ? true : false;
        }

        childNode = node.GetChild("Graphics");
        if (childNode)
        {
            String attribute;

            m_WindowOptions.m_ScreenSize.x_ = childNode.GetInt("width");
            if (m_WindowOptions.m_ScreenSize.x_ < 800)
            { m_WindowOptions.m_ScreenSize.x_ = 800; }
            if (m_WindowOptions.m_ScreenSize.x_ > 3840)
            { m_WindowOptions.m_ScreenSize.x_ = 3840; }


            m_WindowOptions.m_ScreenSize.y_ = childNode.GetInt("height");
            if (m_WindowOptions.m_ScreenSize.y_ < 600)
            { m_WindowOptions.m_ScreenSize.y_ = 600; }
            if (m_WindowOptions.m_ScreenSize.y_ > 2160)
            { m_WindowOptions.m_ScreenSize.y_ = 2160; }

            m_WindowOptions.m_Position.x_ = childNode.GetInt("pos_x");
            m_WindowOptions.m_Position.y_ = childNode.GetInt("pos_y");

            attribute = childNode.GetAttribute("runfullspeed");

            attribute = childNode.GetAttribute("vsync");
            m_WindowOptions.m_bVSync = (attribute == "yes") ? true : false;

            attribute = childNode.GetAttribute("windowMode");
            m_WindowOptions.m_bWindowMode = (attribute == "yes") ? true : false;

            m_WindowOptions.m_Multisample = childNode.GetInt("multisample");

            attribute = childNode.GetAttribute("triplebuffer");
            m_WindowOptions.m_bTripleBuffer = (attribute == "yes") ? true : false;
        }

        childNode = node.GetChild("Renderer");
        if (childNode)
        {
            String attribute;

            attribute = childNode.GetAttribute("materialQuality");
            m_VideoOptions.m_MaterialQuality = GetQuality(attribute);

            attribute = childNode.GetAttribute("textureQuality");
            m_VideoOptions.m_TextureQuality = GetQuality(attribute);

            attribute = childNode.GetAttribute("dynamicInstancing");
            m_VideoOptions.m_bIsDynamicInstancing = (attribute == "yes") ? true : false;

            attribute = childNode.GetAttribute("drawShadows");
            m_VideoOptions.m_bIsDrawShadows = (attribute == "yes") ? true : false;

            attribute = childNode.GetAttribute("shadowQuality");
            m_VideoOptions.m_ShadowQuality = GetShadowQuality(attribute);

            m_VideoOptions.m_ShadowMapSize = childNode.GetInt("shadowMapSize");

            m_VideoOptions.m_bIsSpecularLight = childNode.GetBool("isSpecularLight");
        }

        childNode = node.GetChild("Sound");
        if (childNode)
        {
            String attribute;
            attribute = childNode.GetAttribute("enableSound");
            m_SoundOptions.m_bIsEnableSound = (attribute == "yes") ? true : false;

            m_SoundOptions.m_MasterVolume = childNode.GetInt("masterVolume");
            m_SoundOptions.m_MusicVolume = childNode.GetInt("musicVolume");
            m_SoundOptions.m_SoundEffectsVolume = childNode.GetInt("sfxVolume");
            m_SoundOptions.m_VoiceVolume = childNode.GetInt("voiceVolume");
            m_SoundOptions.m_AmbientVolume = childNode.GetInt("ambientVolume");
        }

        childNode = node.GetChild("ResCache");
        if (childNode)
        {
            attribute = childNode.GetAttribute("useDevelopmentDirectories");
            m_DebugOptions.m_bUseDevelopmentDirectories = ((attribute == "yes") ? (true) : (false));
        }

        childNode = node.GetChild("PhysicsDebug");
        if (childNode)
        {
            String attribute(childNode.GetAttribute("DrawWireFrame"));
            m_PhysicsOptions.m_bIsDrawWireFrame = ((attribute == "yes") ? (true) : (false));

            attribute = childNode.GetAttribute("DrawContactPoints");
            m_PhysicsOptions.m_bIsDrawContactPoints = ((attribute == "yes") ? (true) : (false));

            attribute = childNode.GetAttribute("debugMode");
            m_PhysicsOptions.m_bIsDebugPhysics = ((attribute == "yes") ? (true) : (false));
        }

        childNode = node.GetChild("Debugging");
        if (childNode)
        {
            String attribute(childNode.GetAttribute("useDebugHud"));
            m_DebugOptions.m_bUseDebugHud = ((attribute == "yes") ? (true) : (false));

            attribute = String(childNode.GetAttribute("useConsole"));
            m_DebugOptions.m_bUseConsole = (attribute == "yes") ? true : false;
        }
    }

    InitMultisamples();
    InitResolutions();
    InitShadowQualities();
    InitQualities();
    InitShadowMapSizes();

    URHO3D_LOGINFO("Successfully loaded simulation options");
    return true;
}

bool BaseGameOptions::ChangeXML()
{
    XMLElement root = m_pSimulationOptionFile->GetRoot();
    if (!root)
    {
        return false;
    }
    XMLElement node;
    if (root.GetAttribute("name").Compare("options") == 0)
    {
        node = root.GetChild("UserInterace");
        if (node)
        {
            node.SetAttribute("legacyMode", (m_DebugOptions.m_bIsLegacyMode == true ? "yes" : "no"));
        }

        node = root.GetChild("Sound");
        if (node)
        {
            node.SetAttribute("enableSound", (m_SoundOptions.m_bIsEnableSound == true ? "yes" : "no"));
            node.SetInt("masterVolume", m_SoundOptions.m_MasterVolume);
            node.SetInt("sfxVolume", m_SoundOptions.m_SoundEffectsVolume);
            node.SetInt("musicVolume", m_SoundOptions.m_MusicVolume);
            node.SetInt("voiceVolume", m_SoundOptions.m_VoiceVolume);
            node.SetInt("ambientVolume", m_SoundOptions.m_AmbientVolume);
        }

        node = root.GetChild("Graphics");
        if (node)
        {
            node.SetInt("width", m_WindowOptions.m_ScreenSize.x_);
            node.SetInt("height", m_WindowOptions.m_ScreenSize.y_);
            node.SetInt("pos_x", m_WindowOptions.m_Position.x_);
            node.SetInt("pos_y", m_WindowOptions.m_Position.y_);
            node.SetAttribute("vsync", (m_WindowOptions.m_bVSync == true ? "yes" : "no"));
            node.SetAttribute("windowMode", (m_WindowOptions.m_bWindowMode == true ? "yes" : "no"));

            node.SetInt("multisample", m_WindowOptions.m_Multisample);
            node.SetAttribute("triplebuffer",
                              (m_WindowOptions.m_bTripleBuffer == true ? "yes" : "no"));
        }

        node = root.GetChild("Renderer");
        if (node)
        {
            String attribute;

            node.SetAttribute("materialQuality",
                              GetQualityStr(m_VideoOptions.m_MaterialQuality).ToLower());
            node.SetAttribute("textureQuality",
                              GetQualityStr(m_VideoOptions.m_TextureQuality).ToLower());
            node.SetAttribute("dynamicInstancing",
                              (m_VideoOptions.m_bIsDynamicInstancing == true ? "yes" : "no"));
            node.SetAttribute("drawShadows", (m_VideoOptions.m_bIsDrawShadows == true ? "yes" : "no"));

            node.SetAttribute("shadowQuality",
                              GetShadowQualityStr(m_VideoOptions.m_ShadowQuality).ToLower());
            node.SetInt("shadowMapSize", m_VideoOptions.m_ShadowMapSize);
            node.SetAttribute("isSpecularLight",
                              (m_VideoOptions.m_bIsDrawShadows == true ? "yes" : "no"));
        }

        node = root.GetChild("Multiplayer");
        if (node)
        {
            node.SetInt("listenPort", m_MultiplayerOptions.m_ListenPort);
            node.SetAttribute("gameHost", m_MultiplayerOptions.m_GameHost);
        }

        node = root.GetChild("ResCache");
        if (node)
        {
            node.SetAttribute("useDevelopmentDirectories",
                              (m_DebugOptions.m_bUseDevelopmentDirectories == true ? "yes" : "no"));
        }

        node = root.GetChild("PhysicsDebug");
        if (node)
        {
            node.SetAttribute("DrawWireFrame",
                              (m_PhysicsOptions.m_bIsDrawWireFrame == true ? "yes" : "no"));
            node.SetAttribute("DrawContactPoints",
                              (m_PhysicsOptions.m_bIsDrawContactPoints == true ? "yes" : "no"));
            node.SetAttribute("debugMode",
                              (m_PhysicsOptions.m_bIsDebugPhysics == true ? "yes" : "no"));
        }

        node = root.GetChild("Debugging");
        if (node)
        {
            node.SetAttribute("useDebugHud", (m_DebugOptions.m_bUseDebugHud == true ? "yes" : "no"));
            node.SetAttribute("useConsole", (m_DebugOptions.m_bUseConsole == true ? "yes" : "no"));
        }
    }

    FileSystem* pFileSystem = g_pApp->GetFileSystem();
    String path = g_pApp->GetFileSystem()->GetProgramDir() + "GameData/System/PlayerOptions.xml";
    URHO3D_LOGINFO("BaseGameOptions Save path = " + path);
    if (pFileSystem)
    {
        File saveFile(g_pApp->GetContext(), path, FILE_WRITE);
        if (m_pSimulationOptionFile->Save(saveFile, "\t"))
        {

        }
        else
        {
            URHO3D_LOGINFO("Failed to save BaseGameOptions.xml to " + path);
        }
    }
    else
    {
        URHO3D_LOGINFO("Failed to get FileSystem subsystem");
    }

    return true;
}

IntVector2 BaseGameOptions::GetResolutionValues(String res)
{
    int offset = res.Find("x");
    String ystr;
    ystr.Append(res.CString() + offset + 1);
    int x = atoi(String(res.CString(), offset).CString());
    int y = atoi(ystr.CString());
    return IntVector2(x, y);
}

void BaseGameOptions::InitResolutions()
{
    pugi::xml_document* document = m_pSimulationOptionFile->GetDocument();

    if (!document)
    {
        URHO3D_LOGINFO("Failed to init resolutions in BaseGameOptions xml file");
    }

    pugi::xml_node root = document->root();
    if (!root)
    {
        URHO3D_LOGINFO("Failed to find root in BaseGameOptions xml file");
    }

    pugi::xml_node node;
    node = root.find_child_by_attribute("name", "resolution");
    if (node)
    {
        int counter = 0;
        bool isFound = false;
        for (pugi::xml_node res = node.child("Resolution-1"); res; res = res.next_sibling())
        {
            String width = res.attribute("width").as_string();
            String height = res.attribute("height").as_string();
            int widthInt = ToInt(width);
            int heightInt = ToInt(height);
            m_VideoOptions.m_Resolutions.Push(width + String("x") + height);

            if (!isFound)
            {
                if (widthInt == m_WindowOptions.m_ScreenSize.x_ &&
                    heightInt == m_WindowOptions.m_ScreenSize.y_)
                {
                    m_VideoOptions.m_CurrentSelectedRes = counter;
                    isFound = true;
                }
                counter++;
            }
        }
    }
}

void BaseGameOptions::InitMultisamples()
{
    for (int i = 1; i <= 4; i++)
    {
        int multisample = (int) pow(2, i);
        m_VideoOptions.m_Multisamples.Push(String(multisample));
    }

    int i = (int) Log_2f((float) m_WindowOptions.m_Multisample);
    m_VideoOptions.m_CurrentMultisample = i - 1;
}

void BaseGameOptions::InitShadowMapSizes()
{
    for (int i = 4; i <= 12; i++)
    {
        int shadowmapsize = (int) pow(2, i);
        m_VideoOptions.m_ShadowMapSizes.Push(String(shadowmapsize));
    }
    int i = (int) Log_2f((float) m_VideoOptions.m_ShadowMapSize);
    m_VideoOptions.m_CurrrentSelectedShadowMapSize = i - 1;
}

void BaseGameOptions::InitShadowQualities()
{
    String value = GetShadowQualityStr(SHADOWQUALITY_SIMPLE_16BIT);
    value.Replace("_", " ");
    m_VideoOptions.m_ShadowQualities.Push(value);

    value = GetShadowQualityStr(SHADOWQUALITY_SIMPLE_24BIT);
    value.Replace("_", " ");
    m_VideoOptions.m_ShadowQualities.Push(value);

    value = GetShadowQualityStr(SHADOWQUALITY_PCF_16BIT);
    value.Replace("_", " ");
    m_VideoOptions.m_ShadowQualities.Push(value);

    value = GetShadowQualityStr(SHADOWQUALITY_PCF_24BIT);
    value.Replace("_", " ");
    m_VideoOptions.m_ShadowQualities.Push(value);

    value = GetShadowQualityStr(SHADOWQUALITY_BLUR_VSM);
    value.Replace("_", " ");
    m_VideoOptions.m_ShadowQualities.Push(value);

    value = GetShadowQualityStr(SHADOWQUALITY_VSM);
    value.Replace("_", " ");
    m_VideoOptions.m_ShadowQualities.Push(value);

    String currentShadowQuality = GetShadowQualityStr(m_VideoOptions.m_ShadowQuality);
    currentShadowQuality.Replace("_", " ");
    int count = 0;
    for (auto it = m_VideoOptions.m_ShadowQualities.Begin();
         it != m_VideoOptions.m_ShadowQualities.End(); it++)
    {
        if (currentShadowQuality == (*it))
        {
            m_VideoOptions.m_CurrentSelectedShadowQuality = count;
            break;
        }
        count++;
    }
}

void BaseGameOptions::InitQualities()
{
    String value = GetQualityStr(QUALITY_LOW);
    m_VideoOptions.m_Qualities.Push(value);

    value = GetQualityStr(QUALITY_MEDIUM);
    m_VideoOptions.m_Qualities.Push(value);

    value = GetQualityStr(QUALITY_HIGH);
    m_VideoOptions.m_Qualities.Push(value);

    value = GetQualityStr(QUALITY_MAX);
    m_VideoOptions.m_Qualities.Push(value);

    String currentTextureQuality = GetQualityStr(m_VideoOptions.m_TextureQuality);
    int count = 0;
    for (auto it = m_VideoOptions.m_Qualities.Begin(); it != m_VideoOptions.m_Qualities.End(); it++)
    {
        if (currentTextureQuality == (*it))
        {
            m_VideoOptions.m_CurrentSelectedTextureQuality = count;
            break;
        }
        count++;
    }

    String currentMaterialQuality = GetQualityStr(m_VideoOptions.m_MaterialQuality);
    count = 0;
    for (auto it = m_VideoOptions.m_Qualities.Begin(); it != m_VideoOptions.m_Qualities.End(); it++)
    {
        if (currentMaterialQuality == (*it))
        {
            m_VideoOptions.m_CurrentSelectedMaterialQuality = count;
            break;
        }
        count++;
    }
}

int BaseGameOptions::GetQuality(String quality)
{
    String value = quality.ToLower();
    if (value == "low")
    {
        return QUALITY_LOW;
    }
    else if (value == "medium")
    {
        return QUALITY_MEDIUM;
    }
    else if (value == "high")
    {
        return QUALITY_HIGH;
    }
    else if (value == "max")
    {
        return QUALITY_MAX;
    }
    else
    {
        return QUALITY_LOW;
    }
}

String BaseGameOptions::GetQualityStr(int quality)
{
    switch (quality)
    {
        case QUALITY_LOW:
        {
            return "low";
        }

        case QUALITY_MEDIUM:
        {
            return "medium";
        }

        case QUALITY_HIGH:
        {
            return "high";
        }

        case QUALITY_MAX:
        {
            return "max";
        }

        default:
        {
            return "low";
        }
    }
}

ShadowQuality BaseGameOptions::GetShadowQuality(String quality)
{
    String value = quality.ToLower();
    value.Replace(" ", "_");

    if (value == "simple_16bit")
    {
        return SHADOWQUALITY_SIMPLE_16BIT;
    }
    else if (value == "simple_24bit")
    {
        return SHADOWQUALITY_SIMPLE_24BIT;
    }
    else if (value == "pcf_16bit")
    {
        return SHADOWQUALITY_PCF_16BIT;
    }
    else if (value == "pcf_24bit")
    {
        return SHADOWQUALITY_PCF_24BIT;
    }
    else if (value == "blur_vsm")
    {
        return SHADOWQUALITY_BLUR_VSM;
    }
    else if (value == "vsm")
    {
        return SHADOWQUALITY_VSM;
    }
    else
    {
        return SHADOWQUALITY_SIMPLE_16BIT;
    }
}

String BaseGameOptions::GetShadowQualityStr(ShadowQuality quality)
{
    switch (quality)
    {
        case SHADOWQUALITY_SIMPLE_16BIT:
        {
            return "Simple_16bit";
        }

        case SHADOWQUALITY_SIMPLE_24BIT:
        {
            return "Simple_24bit";
        }

        case SHADOWQUALITY_PCF_16BIT:
        {
            return "Pcf_16bit";
        }

        case SHADOWQUALITY_PCF_24BIT:
        {
            return "Pcf_24bit";
        }


        case SHADOWQUALITY_BLUR_VSM:
        {
            return "Blur_vsm";
        }

        case SHADOWQUALITY_VSM:
        {
            return "Vsm";
        }

        default:
        {
            return "Simple_16bit";
        }
    }
}