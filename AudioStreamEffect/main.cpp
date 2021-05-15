#include <iostream>
#include <mutex>

#include "Window.h"
#include "Track.h"
#include "AudioDevice.h"
#include "imgui.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include "SpeedStreamTransform.h"
#include "FilterStreamTransform.h"

std::mutex g_Mutex;

struct AudioStreamUser
{
	std::unique_ptr<StreamTransform>& transform;
	std::unique_ptr<Track>& track;
};

static void AudioStreamFunc(float* out, size_t pos, size_t count, void* user)
{
	std::scoped_lock lock(g_Mutex);
	AudioStreamUser& u = *(AudioStreamUser*)user;
	if (u.track.get() == nullptr || !u.track->Playing())
	{
		for (unsigned int i = 0; i < count; i++)
		{
			*out++ = 0.0;
			*out++ = 0.0;
		}
	}
	else
	{
		Track& track = *u.track.get();
		SampleView left_view;
		SampleView right_view;
		if (u.transform.get() == nullptr)
		{
			double position = track.GetPosition();
			left_view = track.GetSampleView((size_t)position, count, 0);
			right_view = track.GetSampleView((size_t)position, count, 1);
			u.track->AdvancePosition((double)count);
		}
		else
		{
			StreamTransform& st = *u.transform.get();
			left_view = st.Transform(track, pos, count, 0);
			right_view = st.Transform(track, pos, count, 1);
			st.EndTransform(track, pos, count);
		}

		for (unsigned int i = 0; i < count; i++)
		{
			*out++ = left_view[i] * (float)u.track->Volume();
			*out++ = right_view[i] * (float)u.track->Volume();
		}
	}
}

void Update(void* user);

int main()
{
	std::unique_ptr<StreamTransform> st;
	std::unique_ptr<Track> track;
	AudioStreamUser user{ st, track };

	Window window(Update, &user);

	AudioDevice audio_device(AudioStreamFunc, &user);
	audio_device.Play();

	window.Run();
}

void Update(void* user)
{
	std::scoped_lock lock(g_Mutex);
	AudioStreamUser& u = *(AudioStreamUser*)user;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("file"))
		{
			if (ImGui::MenuItem("import"))
			{
				char const* filters[] = { "*.wav" };
				char const* result = tinyfd_openFileDialog(NULL, NULL, (int)std::size(filters), filters, "sound files", 0);
				if (result != NULL)
					u.track = OpenTrack(result);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("speed"))
		{
			NearestNeighbourSST temp;
			const SpeedStreamTransform& base =
				(u.transform.get() != nullptr && dynamic_cast<SpeedStreamTransform*>(u.transform.get()) != nullptr) ?
				*static_cast<SpeedStreamTransform*>(u.transform.get()) :
				*static_cast<SpeedStreamTransform*>(&temp);

			if (ImGui::MenuItem("nearest neighbour"))
				u.transform = std::make_unique<NearestNeighbourSST>(base);
			if (ImGui::MenuItem("linear"))
				u.transform = std::make_unique<LinearSST>(base);
			if (ImGui::MenuItem("cubic"))
				u.transform = std::make_unique<CubicSST>(base);
			if (ImGui::MenuItem("windowed sinc"))
				u.transform = std::make_unique<WindowedSincSST>(base);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("filter"))
		{
			LowPassFST temp;
			const FilterStreamTransform& base =
				(u.transform.get() != nullptr && dynamic_cast<FilterStreamTransform*>(u.transform.get()) != nullptr) ?
				*static_cast<FilterStreamTransform*>(u.transform.get()) :
				*static_cast<FilterStreamTransform*>(&temp);

			if (ImGui::MenuItem("low pass"))
				u.transform = std::make_unique<LowPassFST>(base);
			if (ImGui::MenuItem("high pass"))
				u.transform = std::make_unique<HighPassFST>(base);
			if (ImGui::MenuItem("band pass"))
				u.transform = std::make_unique<BandPassFST>(base);

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (u.transform.get() != nullptr)
		if (!u.transform->Draw())
			u.transform.reset();

	if (u.track.get() != nullptr)
		if (!u.track->Draw())
			u.track.reset();
}
