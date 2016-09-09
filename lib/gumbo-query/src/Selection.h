/***************************************************************************
 * 
 * $Id$
 * 
 **************************************************************************/

/**
 * @file $HeadURL$
 * @author $Author$(hoping@baimashi.com)
 * @date $Date$
 * @version $Revision$
 * @brief 
 *  
 **/

#ifndef SELECTION_H_
#define SELECTION_H_

#include "Object.h"
#include <vector>
#include <string>
#include <exception>
#include <gumbo.h>

class CNode;

class CSelection: public CObject
{

	public:

		CSelection(GumboNode* apNode);

		CSelection(std::vector<GumboNode*> aNodes);

		virtual ~CSelection();

	public:

		CSelection find(std::string aSelector);

		CNode nodeAt(size_t i);

		size_t nodeNum();

		CSelection assertNum(const int num) {
			if (nodeNum() != num)
				throw std::runtime_error(std::string("selection has ") + std::to_string(nodeNum()) + " nodes instead of " + std::to_string(num));
			return *this;
		}

		CSelection assertAtLeast(const int num) {
			if (nodeNum() < num)
				throw std::runtime_error(std::string("selection has ") + std::to_string(nodeNum()) + " nodes instead of at least " + std::to_string(num));
			return *this;
		}

		std::vector<GumboNode*>::const_iterator begin() {
			return mNodes.begin();
		}

		std::vector<GumboNode*>::const_iterator end() {
			return mNodes.end();
		}

	private:

		std::vector<GumboNode*> mNodes;
};

#endif /* SELECTION_H_ */

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
