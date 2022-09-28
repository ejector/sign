#ifndef COMMANDLINEAPP_H
#define COMMANDLINEAPP_H

class CommandLineApp final
{
public:
    explicit CommandLineApp(int argc, char** argv);
    [[nodiscard]] int Exec() const;

public:
    static constexpr int kExitWithError = 1;

protected:
    int argc_;
    char** argv_;
};

#endif // COMMANDLINEAPP_H
