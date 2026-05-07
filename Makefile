# Compiler and flags
CXX := clang++
PKG_CONFIG := pkg-config

OPT_FLAGS := -O3 -march=native -mtune=native -funroll-loops -fvectorize -ffp-contract=fast -freciprocal-math -ffast-math -fstrict-aliasing -flto=full -mprefer-vector-width=256 -fomit-frame-pointer

CXXFLAGS := -std=c++23 -Wall -Werror -Wextra $(OPT_FLAGS)
CPPFLAGS := -Iincludes $(shell $(PKG_CONFIG) --cflags raylib)
LDLIBS := -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXi -lXcursor -lXinerama

# Directories
SRCDIR := srcs
OBJDIR := .objs

# Target binary
TARGET := Rubiks

# Source and object files
SRCFILES := $(wildcard $(SRCDIR)/*.cpp)
OBJFILES := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCFILES))

# Default target
all: check-raylib $(TARGET)

# Check raylib availability
check-raylib:
	@$(PKG_CONFIG) --exists raylib || (echo "raylib not found. Install raylib and pkg-config."; exit 1)

# Link object files into final binary
$(TARGET): $(OBJFILES)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) $(OBJFILES) -o $(TARGET) $(LDLIBS)

# Compile .cpp to .o
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

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

.PHONY: all clean fclean re check-raylib
