#define EXPAND(x) x
#define CONCAT(a,b) EXPAND(a)b

#define INTERFACE_NAME CONCAT(WIDGET_NAME,Interface)
#define AS_STRING_IMPL(x) #x
#define AS_STRING(x) AS_STRING_IMPL(x)

#define INTERFACE_PATH(n) CONCAT(INTERFACE_NAME,.h)
#define INTERFACE_FILE AS_STRING(INTERFACE_PATH(INTERFACE_NAME))

#define WIDGET_DIRECTORY widgets/
#define WIDGET_FILE AS_STRING(CONCAT(WIDGET_DIRECTORY,CONCAT(WIDGET_NAME,.h)))

#define DEFINE_DEFAULT_CONSTRUCTION INTERFACE_NAME::INTERFACE_NAME(QObject *parent) : QObject(parent) {}
#define DEFINE_DEFAULT_INITIALIZATION void INTERFACE_NAME::initialize(QDesignerFormEditorInterface* /*core*/) { if (initialized) return; initialized = true; }

#define DEFINE_DEFAULT_WIDGET_CONSTRUCTOR QWidget* INTERFACE_NAME::createWidget(QWidget* parent) { return new WIDGET_NAME(parent); }
#define DEFINE_DEFAULT_GROUP_GETTER QString INTERFACE_NAME::group() const { return "ReachVariantTool"; }
#define DEFINE_DEFAULT_ICON_GETTER QIcon INTERFACE_NAME::icon() const { return {}; }
#define DEFINE_DEFAULT_NAME_GETTER QString INTERFACE_NAME::name() const { return AS_STRING(WIDGET_NAME); }
#define DEFINE_TOOLTIP_TEXT_GETTER(text) QString INTERFACE_NAME::toolTip() const { return text; }
#define DEFINE_WHATS_THIS_TEXT_GETTER(text) QString INTERFACE_NAME::whatsThis() const { return text; }

#define DEFINE_BOILERPLATE_GETTERS(_default_widget_id) \
   QString INTERFACE_NAME::domXml() const { \
      return "<widget class=\"" AS_STRING(WIDGET_NAME) "\" name=\"" #_default_widget_id "\"></widget>\n"; \
   } \
   \
   QString INTERFACE_NAME::includeFile() const { \
      return WIDGET_FILE; \
   }