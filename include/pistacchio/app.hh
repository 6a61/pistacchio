#pragma once

#include <cstdint>

// Holds state for a real-time application that runs its simulation at a fixed
// rate and tries to render at a certain target rate (can be disabled, making
// it render as fast as it can).
class App {
protected:
	double m_fixed_rate;
	double m_target_rate;
	bool m_stop = false;
	uint64_t m_time = 0;
public:
	App(double fixed_rate, double target_rate);
	virtual ~App();

	void fixed_rate(double value);
	void target_rate(double value);

	// Starts running application.
	virtual void start();

	// Stops application.
	virtual void stop();

protected:
	// Gets called as much as it can.
	virtual void update(double dt);

	// Gets called at a fixed rate of `m_fixed_rate`.
	virtual void fixed_update(double dt);

	// Gets called at a rate of `m_target_rate` or as fast as it can if
	// it's 0.
	virtual void render(double alpha);
};
