#pragma once

#include <mm/engine.hpp>
#include <vector>
#include <set>

namespace MM::Services {

	class NetChanneledInterface : public Service {
		public:
			using channel_id = uint8_t;
			enum class channel_type {
				LOSSY,
				LOSSLESS
			};
			using peer_id = uint64_t; // might be truncated by backends

			using packet_type = uint16_t; // can be aliased to an enum by the user

		protected:
			std::set<peer_id> _peer_list;

		public: // support querrys

			// returns the number (highest number??) maximum channels supported by backend
			virtual channel_id getMaxChannels(void) = 0;

			// returns wether the channel type is supported
			virtual bool getSupportedChannelType(channel_type type) = 0;

			// in the best case, this performs a propper MTU estimation (lol)
			virtual size_t getMaxPacketSize(void) = 0;

			// TODO: add set channel type utils

		public:

			// ok , we manage the peer list now
			virtual bool addPeer(peer_id peer) { return _peer_list.emplace(peer).second; }
			virtual bool removePeer(peer_id peer) { return _peer_list.erase(peer); }
			virtual void clearPeerlist(void) { _peer_list.clear(); }

			// calls fn for each peer
			virtual void forEachPeer(std::function<void(peer_id)> fn) {
				for (peer_id peer : _peer_list) {
					fn(peer);
				}
			}

		public: // send/recv

			// sends a packet of max getMaxPacketSize() bytes
			virtual bool sendPacket(peer_id peer, channel_id channel, uint8_t* data, size_t data_size) = 0;

			// sends a packet, automatically split if too big
			// !! only on lossless channels
			virtual bool sendPacketLarge(peer_id peer, channel_id channel, uint8_t* data, size_t data_size) = 0;

			// TODO: broadcast?
			// has any?

			//virtual bool getPacket

			// calls fn for each packet and fills in peer, channel, data_ptr, and data_size
			// returns number of fn calls
			virtual size_t forEachPacket(std::function<void(peer_id, channel_id, uint8_t*, size_t)> fn) = 0;

			// calls fn for each packet and fills in channel, data_ptr, and data_size
			// returns number of fn calls
			virtual size_t forEachPacketPeer(peer_id peer, std::function<void(peer_id, channel_id, uint8_t*, size_t)> fn) = 0;

			// calls fn for each packet and fills in data_ptr, and data_size
			// returns number of fn calls
			virtual size_t forEachPacketPeerChannel(peer_id peer, channel_id channel, std::function<void(peer_id, channel_id, uint8_t*, size_t)> fn) = 0;
	};

} // MM::Services

