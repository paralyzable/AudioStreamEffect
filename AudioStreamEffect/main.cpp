#include <iostream>
#include <mutex>
#include <algorithm>

#include "Window.h"
#include "Track.h"
#include "AudioDevice.h"
#include "imgui.h"
#include "tinyfiledialogs/tinyfiledialogs.h"

#include "stream_transform/filter/frequency_domain/Filters.h"
#include "stream_transform/filter/time_domain/Filters.h"
#include "stream_transform/speed/SpeedTransforms.h"

std::mutex g_Mutex;

struct AudioStreamUser
{
	std::unique_ptr<StreamTransform>& transform;
	Track& track;
};

static void AudioStreamFunc(float* out, size_t pos, size_t count, void* user);

void Update(void* user);

int main()
{
	std::unique_ptr<StreamTransform> st;
	Track track;
	AudioStreamUser user{ st, track };

	Window window(Update, &user);

	AudioDevice audio_device(AudioStreamFunc, &user);
	audio_device.Play();

	window.Run();
}

static void AudioStreamFunc(float* out, size_t pos, size_t count, void* user)
{
	std::scoped_lock lock(g_Mutex);
	AudioStreamUser& u = *(AudioStreamUser*)user;
	Track& track = u.track;
	if (!track.IsValid() || !track.Playing())
	{
		for (size_t i = 0; i < count; i++)
		{
			*out++ = 0.0f;
			*out++ = 0.0f;
		}
	}
	else
	{
		SampleView left_view;
		SampleView right_view;
		if (u.transform.get() == nullptr)
		{
			double position = track.GetPosition();
			left_view = track.GetSampleView((size_t)position, count, 0);
			right_view = track.GetSampleView((size_t)position, count, 1);
			track.AdvancePosition((double)count);
		}
		else
		{
			StreamTransform& st = *u.transform.get();
			left_view = st.Transform(track, pos, count, 0);
			right_view = st.Transform(track, pos, count, 1);
			st.EndTransform(track, pos, count);
		}

		for (size_t i = 0; i < count; i++)
		{
			*out++ = left_view[i] * track.Gain();
			*out++ = right_view[i] * track.Gain();
		}
	}
}

void Update(void* user)
{
	std::scoped_lock lock(g_Mutex);
	AudioStreamUser& u = *(AudioStreamUser*)user;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("track"))
		{
			if (ImGui::MenuItem("import"))
			{
				char const* filters[] = { "*.wav" };
				char const* result = tinyfd_openFileDialog(NULL, NULL, (int)std::size(filters), filters, "sound files", 0);
				if (result != NULL)
					u.track = Track(result);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("speed"))
		{
			NearestNeighbourSST temp;
			SpeedStreamTransform& base =
				(u.transform.get() != nullptr && dynamic_cast<SpeedStreamTransform*>(u.transform.get()) != nullptr) ?
				*static_cast<SpeedStreamTransform*>(u.transform.get()) :
				*static_cast<SpeedStreamTransform*>(&temp);

			if (ImGui::MenuItem("nearest neighbour"))
				u.transform = std::make_unique<NearestNeighbourSST>(std::move(base));
			if (ImGui::MenuItem("linear"))
				u.transform = std::make_unique<LinearSST>(std::move(base));
			if (ImGui::MenuItem("cubic"))
				u.transform = std::make_unique<CubicSST>(std::move(base));
			if (ImGui::MenuItem("windowed sinc"))
				u.transform = std::make_unique<WindowedSincSST>(std::move(base));

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("filter"))
		{
			if (ImGui::BeginMenu("time domain"))
			{
				LowPassTF temp;
				TimeDomainFilter& base =
					(u.transform.get() != nullptr && dynamic_cast<TimeDomainFilter*>(u.transform.get()) != nullptr) ?
					*static_cast<TimeDomainFilter*>(u.transform.get()) :
					*static_cast<TimeDomainFilter*>(&temp);

				if (ImGui::MenuItem("low pass"))
					u.transform = std::make_unique<LowPassTF>(std::move(base));
				if (ImGui::MenuItem("high pass"))
					u.transform = std::make_unique<HighPassTF>(std::move(base));
				if (ImGui::MenuItem("band pass"))
					u.transform = std::make_unique<BandPassTF>(std::move(base));
				if (ImGui::MenuItem("band reject"))
					u.transform = std::make_unique<BandRejectTF>(std::move(base));

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("frequency domain"))
			{
				LowPassFF temp;
				FrequencyDomainFilter& base =
					(u.transform.get() != nullptr && dynamic_cast<FrequencyDomainFilter*>(u.transform.get()) != nullptr) ?
					*static_cast<FrequencyDomainFilter*>(u.transform.get()) :
					*static_cast<FrequencyDomainFilter*>(&temp);

				if (ImGui::MenuItem("low pass"))
					u.transform = std::make_unique<LowPassFF>(std::move(base));
				if (ImGui::MenuItem("high pass"))
					u.transform = std::make_unique<HighPassFF>(std::move(base));
				if (ImGui::MenuItem("band pass"))
					u.transform = std::make_unique<BandPassFF>(std::move(base));
				if (ImGui::MenuItem("band reject"))
					u.transform = std::make_unique<BandRejectFF>(std::move(base));

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("pitch"))
		{
			if (ImGui::MenuItem("todo"));
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (u.transform.get() != nullptr)
		if (!u.transform->Draw())
			u.transform.reset();

	if (u.track.IsValid())
		if (!u.track.Draw())
			u.track = Track();
}
