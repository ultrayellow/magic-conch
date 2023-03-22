## ******************************* ##
##            @COPYLEFT            ##
##       ALL WRONGS RESERVED       ##
## ******************************* ##

TARGET = magic_conch.out
OBJECTS_DIR = objs/
CXXFLAGS += -Ishared_ptr
LDFLAGS = -lreadline

CXXFLAGS += -Wall -Wextra -Werror -std=c++11 -pedantic

SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(OBJECTS_DIR), $(SOURCES:.cpp=.o))

CXXFLAGS += -fsanitize=address -g3

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)

$(OBJECTS_DIR)%.o: %.cpp | $(OBJECTS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MMD -MF $(@:.o=.d) -MT $@ -MP

.PHONY: clean
clean:
	rm -rf $(OBJECTS_DIR)

-include $(OBJECTS:.o=.d)
