# Compiler and flags
CXX := clang++
CXXFLAGS := -std=c++23 -Wall -Werror -Wextra -O3 -g

# Directories
SRCDIR := srcs
INCDIR := -Iincludes
OBJDIR := .objs

# Target binary
TARGET := Rubiks

# Source and object files
SRCFILES := $(wildcard $(SRCDIR)/*.cpp)
OBJFILES := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCFILES))

# Default target
all: $(TARGET)

# Link object files into final binary
$(TARGET): $(OBJFILES)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJFILES) -o $(TARGET) $(LDFLAGS)

# Compile .cpp to .o
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCDIR) -c $< -o $@

# Create object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean build files
clean:
	rm -rf $(OBJDIR)

# Clean everything including binary
fclean: clean
	rm -f $(TARGET)

# Rebuild from scratch
re: fclean all

.PHONY: all clean fclean re

