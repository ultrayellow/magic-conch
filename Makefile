## ******************************* ##
##            @COPYLEFT            ##
##       ALL WRONGS RESERVED       ##
## ******************************* ##

TARGET = microshellxx.out
OBJECTS_DIR = objs/
CXXFLAGS = -lreadline

SOURCES = shell.cpp token.cpp
OBJECTS = $(addprefix $(OBJECTS_DIR), $(SOURCES:.cpp=.o))

CXXFLAGS += -fsanitize=address -g3

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJECTS_DIR):
	mkdir $(OBJECTS_DIR)

$(OBJECTS_DIR)%.o: %.cpp | $(OBJECTS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MMD -MF $(@:.o=.d) -MT $@ -MP

-include $(OBJECTS:.o=.d)