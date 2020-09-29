#pragma once

#include <cstdint>

namespace MM::OpenGL {

	class VertexArrayObject {
		private:
			uint32_t _rendererID;

		public:
			VertexArrayObject(void);
			~VertexArrayObject(void);

			void bind(void) const;
			void unbind(void) const;
	};

} // MM::OpenGL

